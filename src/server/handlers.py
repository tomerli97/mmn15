import client
from protocol import *
from message import Message
import logging

def handle_register(conn, req):
    logging.info('Handling register')
    new_client = client.register(req.name, req.pubkey)
    RegisterSuccessResponse(new_client).send(conn)

def handle_list_users(conn, req):
    logging.info('Handling list_users')
    UserListResponse(client.get_client_list(), client.get_client(req.client_id)).send(conn)

def handle_get_user_pubkey(conn, req):
    logging.info('Handling get_user_pubkey')
    GetUserPubkeyResponse(client.get_client(req.requested_id)).send(conn)

def handle_send_message(conn, req):
    logging.info('Handling send_message')
    m = Message(req.dest_id, req.client_id, req.message_type, req.message_content) # Constructor sends message
    SendMessageResponse(req.dest_id, m.message_id).send(conn)

def handle_recv_messages(conn, req):
    logging.info('Handling recv_messages')
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