import os
import logging
import datetime
import time
import sql
from message import Message

g_clients = {}

MAX_COLLISION_ATTEMPTS = 20

def generate_id():
    for i in range(MAX_COLLISION_ATTEMPTS):
        client_id = os.urandom(16) # Quality random from OS
        if client_id not in g_clients.keys():
            return client_id
        logging.critical(f'Randomly got duplicate id: {client_id.hex()}')
    raise RuntimeError(f'Randomly collided client ids {MAX_COLLISION_ATTEMPTS} times in a row')


def get_current_time():
    return datetime.datetime.fromtimestamp(time.time())


g_db = sql.ServerDB()



class Client:
    def __init__(self, name, pubkey, client_id=None, last_seen=None):
        self.name = name
        self.pubkey = pubkey
        if client_id is None:
            self.client_id = generate_id()
        else:
            self.client_id = client_id

        if last_seen is None:
            self.update_last_seen()
        else:
            self.last_seen = last_seen

        self.pending_messages = []

    def update_last_seen(self):
        self.last_seen = get_current_time()
        g_db.client_seen(self.client_id, self.last_seen)

    def append_message(self, message):
        self.pending_messages.append(message)

    def clear_messages(self):
        for m in self.pending_messages:
            m.consume()
        self.pending_messages = []

def register(name, pubkey):
    logging.info(f'Creating client {name} with pubkey {pubkey}')
    global g_clients

    assert name not in [c.name for c in g_clients.values()], "Cannot create user with existing name"

    c = Client(name, pubkey)
    g_clients[c.client_id] = c
    g_db.register_client(c.client_id, c.name, c.pubkey, c.last_seen)
    logging.info(f'Created client {name} with id {c.client_id}')
    return c

def get_client(client_id):
    return g_clients[client_id]

def get_client_list():
    return g_clients.values()

def load_db():
    for client_id, name, pubkey, last_seen in g_db.get_registered_clients():
        g_clients[client_id] = Client(name, pubkey, client_id, last_seen)

    for message_id, dest_id, from_id, message_type, content in g_db.get_messages():
        Message(dest_id, from_id, int(message_type), content, message_id) # Constructor sends message

