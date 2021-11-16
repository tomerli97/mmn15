import os
import logging
import datetime
import time

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

class Client:
    def __init__(self, name, pubkey):
        self.name = name
        self.pubkey = pubkey #TODO
        self.client_id = generate_id()
        self.update_last_seen()
        self.pending_messages = []

    def update_last_seen(self):
        self.last_seen = get_current_time()

    def append_message(self, message):
        self.pending_messages.append(message)

    def clear_messages(self):
        self.pending_messages = []

def register(name, pubkey):
    logging.info(f'Creating client {name} with pubkey {pubkey}')
    c = Client(name, pubkey)
    global g_clients
    g_clients[c.client_id] = c
    logging.info(f'Created client {name} with id {c.client_id}')
    return c

def get_client(client_id):
    return g_clients[client_id]

def get_client_list():
    return g_clients.values()