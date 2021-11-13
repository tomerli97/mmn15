import client
from protocol import *
from message import Message

def handle_register(conn, req):
    print('Handling register')
    new_client = client.register(req.name, req.pubkey)
    RegisterSuccessResponse(new_client).send(conn)

def handle_list_users(conn, req):
    print('Handling list users')
    UserListResponse(client.get_client_list(), client.get_client(req.client_id)).send(conn)
    print('Sent response')

def handle_get_user_pubkey(conn, req):
    GetUserPubkeyResponse(client.get_client(req.requested_id)).send(conn)

def handle_send_message(conn, req):
    m = Message(req.dest_id, req.client_id, req.message_type, req.message_content) # Constructor sends message
    SendMessageResponse(req.dest_id, m.message_id).send(conn)

def handle_recv_messages(conn, req):
    c = client.get_client(req.client_id)
    RecieveMessagesResponse(c.pending_messages).send(conn)
    c.clear_messages()

HANDLERS = {
    1000: handle_register,
    1001: handle_list_users,
    1002: handle_get_user_pubkey,
    1003: handle_send_message,
    1004: handle_recv_messages,
}