import socket
import logging
import select

from protocol import parse_request, GeneralErrorResponse
from handlers import HANDLERS

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
    except Exception as e:
        logging.exception('Got exception while handling client')
        send_general_error(conn, e)
    finally:
        conn.close()


def main():
    server_port = load_config()
    server_socket = create_listening_socket(server_port)

    while True:
        ready, _, _ = select.select([server_socket], [], [])
        assert server_socket in ready
        conn, addr = server_socket.accept()

        logging.info(f'Got connection from {addr}')
        handle_client(conn)



if __name__ == '__main__':
    main()