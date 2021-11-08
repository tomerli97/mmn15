import struct
from consts import *

class Request:
    def __init__(self, client_id, version, code, size, payload):
        self.client_id = client_id
        self.version = version
        self.code = code
        self.size = size
        self.parse_payload(payload)

    def parse_payload(self, payload):
        self.payload = payload

class RegisterRequest(Request):
    def parse_payload(self, payload):
        name, pubkey = struct.unpack('<255s160s', payload)
        assert '\x00' in name, 'Invalid name - not null terminated'
        self.name = name.split('\x00')[0]
        self.pubkey = pubkey #TODO: Parse key


class UserListRequest(Request):
    def parse_payload(self, payload):
        assert len(payload) == 0, 'Invalid payload for User List request'

class GetUserPubkeyRequest(Request):
    def parse_payload(self, payload):
        self.requested_id = struct.unpack('<16s', payload)

class SendMessageRequest(Request):
    TYPE_KEY_REQUEST = 1
    TYPE_KEY_RESPONSE = 2
    TYPE_TEXT = 3
    TYPE_FILE = 4

    def parse_payload(self, payload):
        message_format = '<16sBI'
        message_header_size = struct.calcsize(message_format)
        self.dest_id, self.message_type, self.content_size = struct.unpack(message_format, data[:message_header_size])
        self.message_content = payload[message_header_size:]

        if self.message_type == TYPE_KEY_REQUEST:
            assert self.content_size == 0, 'Invalid content size for key request'
        elif self.message_type == TYPE_KEY_RESPONSE:
            assert self.content_size == SYM_KEY_LEN, 'Invalid content size for key response' 
        assert len(self.message_content) == self.content_size, 'Content size mismatch'

class RecieveMessagesRequest(Request):
    def parse_payload(self, payload):
        assert len(payload) == 0, 'Invalid payload for Recieve Messages request'


REQUESTS = {
    1000: RegisterRequest,
    1001: UserListRequest,
    1002: GetUserPubkeyRequest,
    1003: SendMessageRequest,
    1004: RecieveMessagesRequest,
}

def recvall(s, size):
    data = b''
    while len(data) < size:
        data += s.recv(size - len(data))
    return data

def parse_request(conn):
    header_format = '<16sBHI'
    header_size = struct.calcsize(header_format)
    header_data = recvall(conn, header_size)
    client_id, version, code, size = struct.unpack(header_format, header_data)
    payload = recvall(conn, size)
    return REQUESTS[code](client_id, version, code, size, payload)


class Response:
    def __init__(self, version = SERVER_VERSION):
        self.version = version
        self.code = None

    def build(self):
        payload = self.build_payload()
        return struct.pack('<BHI', self.version, self.code, len(payload)) + payload

    def build_payload(self):
        raise NotImplementedError()

    def send(self, conn):
        conn.send(self.build())


class RegisterSuccessResponse(Response):
    def __init__(self, client):
        super().__init__()
        self.code = 2000
        self.client_id = client.client_id

    def build_payload(self):
        return self.client_id


class UserListResponse(Response):
    def __init__(self, clients, requester):
        super().__init__()
        self.code = 2001
        self.clients = clients
        self.requester = requester

    def build_payload(self):
        return b''.join(struct.pack('<16s255s', c.client_id, c.name) for c in self.clients if c is not self.requester)

class GetUserPubkeyResponse(Response):
    def __init__(self, client):
        super().__init__()
        self.code = 2002
        self.client = client

    def build_payload(self):
        return struct.pack('<16s160s', self.client.client_id, self.client.pubkey) # TODO: pubkey is not bytes


class SendMessageResponse(Response):
    def __init__(self, recipient_id, message_id):
        super().__init__()
        self.code = 2003
        self.recipient_id = recipient_id
        self.message_id = message_id

    def build_payload(self):
        return struct.pack('<16s4s', self.recipient_id, self.message_id)


class RecieveMessagesResponse(Response):
    def __init__(self, messages):
        super().__init__()
        self.code = 2004
        self.messages = messages

    def build_payload(self):
        def encode_message(m):
            return struct.pack('<16s4sBI', m.sender_id, m.message_id, m.message_type, len(m.content)) + m.content

        return b''.join(encode_message(m) for m in self.messages)

class GeneralErrorResponse(Response):
    def __init__(self):
        super().__init__()
        self.code = 9000

    def build_payload(self):
        return b''