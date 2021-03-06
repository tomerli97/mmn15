import socket
import logging
import select
import client

from protocol import parse_request, GeneralErrorResponse, ConnectionEndedException
from handlers import HANDLERS

logging.basicConfig(level=logging.INFO)

def load_config():
    with open('port.info', 'r') as f:
        return int(f.read())


def create_listening_socket(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', port))
    s.listen(10)
    logging.info(f'Server listening on port {port}')
    return s



def send_general_error(conn, e):
    # Sending specific errors is not supported so e is ignored
    conn.send(GeneralErrorResponse().build())


def handle_client(conn):
    req = parse_request(conn)
    try:
        HANDLERS[req.code](conn, req)
    except ConnectionEndedException:
        raise
    except AssertionError as e:
        logging.info(f'Server assertion: {e.args[0]}')
        send_general_error(conn, e)
    except Exception as e:
        logging.exception('Got exception while handling client')
        send_general_error(conn, e)


def main():
    server_port = load_config()
    client.load_db()
    server_socket = create_listening_socket(server_port)
    clients = []

    while True:
        ready, _, _ = select.select([server_socket] + clients, [], [])
        if server_socket in ready:
            conn, addr = server_socket.accept()
            logging.info(f'Got connection from {addr}')

            clients.append(conn)
        for conn in ready:
            if conn not in clients:
                continue
            logging.info(f'Handling socket {conn.getpeername()}')
            try:
                handle_client(conn)
            except (ConnectionEndedException, ConnectionResetError):
                logging.info(f'Removing connection {conn.getpeername()}')
                clients.remove(conn)



if __name__ == '__main__':
    main()