import client
import os

class Message:
    def __init__(self, to_client, sender_id, message_type, content):
        self.sender_id = sender_id
        self.message_type = message_type
        self.content = content
        self.message_id = os.urandom(4)

        client.g_clients[to_client].append_message(self)