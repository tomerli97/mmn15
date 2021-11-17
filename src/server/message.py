import client
import os

MAX_COLLISION_ATTEMPTS = 20

g_taken_ids = []

def generate_id():
    global g_taken_ids

    for i in range(MAX_COLLISION_ATTEMPTS):
        message_id = os.urandom(4) # Quality random from OS
        if message_id not in g_taken_ids:
            return message_id
    raise RuntimeError(f'Randomly collided client ids {MAX_COLLISION_ATTEMPTS} times in a row')

class Message:
    def __init__(self, to_client, sender_id, message_type, content, message_id=None):
        self.sender_id = sender_id
        self.message_type = message_type
        self.content = content
        if message_id is None:
            self.message_id = generate_id()
            # New message - add to db
            client.g_db.add_message(self.message_id, to_client, sender_id, message_type, content)
        else:
            self.message_id = message_id

        global g_taken_ids
        g_taken_ids.append(self.message_id)

        client.g_clients[to_client].append_message(self)

    def consume(self):
        global g_taken_ids
        g_taken_ids.remove(self.message_id)
        client.g_db.clear_message(self.message_id)
