import sqlite3
import os

DB_PATH = 'server.db'

class ServerDB:
    def __init__(self, db=DB_PATH):
        self._is_init = False
        if os.path.isfile(db):
            self._is_init = True
        self._conn = sqlite3.connect(db)
        self._conn.text_factory = bytes

        if not self._is_init:
            self._initialize()
            self._is_init = True

    def _get_output(self, query):
        cur = self._conn.cursor()
        cur.execute(query)
        return cur.fetchall()

    def _initialize(self):
        self._conn.executescript('''
            CREATE TABLE clients(ID CHAR(16) NOT NULL PRIMARY KEY,
                                Name VARCHAR(255) NOT NULL,
                                PublicKey CHAR(160) NOT NULL,
                                LastSeen TIMESTAMP);
            CREATE TABLE messages(ID CHAR(4) NOT NULL PRIMARY KEY,
                                ToClient CHAR(16) NOT NULL,
                                FromClient CHAR(16) NOT NULL,
                                Type CHAR(1) NOT NULL,
                                Content BLOB NOT NULL);
            ''')
        self._conn.commit()

    def register_client(self, id_, name, pubkey, last_seen):
        self._conn.execute('''
            INSERT INTO clients VALUES(?, ?, ?, ?)
            ''', (id_, name, pubkey, last_seen))
        self._conn.commit()

    def get_registered_clients(self):
        clients = self._get_output('''
            SELECT * FROM clients
            ''')
        return clients

    def add_message(self, id_, to, from_, type_, content):
        self._conn.execute('''
            INSERT INTO messages VALUES(?, ?, ?, ?, ?)
            ''', (id_, to, from_, type_, content))
        self._conn.commit()

    def get_messages(self):
        messages = self._get_output('''
            SELECT * FROM messages
            ''')
        return messages

    def clear_message(self, id_):
        self._conn.execute('''
            DELETE FROM messages
            WHERE ID = ?
            ''', (id_,))
        self._conn.commit()

    def client_seen(self, id_, last_seen):
        self._conn.execute('''
            UPDATE clients 
            SET LastSeen = ?
            WHERE id = ?
            ''', (last_seen, id_))
        self._conn.commit()