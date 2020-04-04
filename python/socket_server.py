
import socket


def server():
    global port
    host = "192.168.0.174"
    comms_socket = socket.socket()
    comms_socket.bind((host, 9000))
    print("Waiting for a char at", host, "on port", port)
    comms_socket.listen(10)
    send_data = ""
    while True:
        connection, address = comms_socket.accept()

        print("open chat with", address)
        while send_data != "EXIT":
            print(connection.recv(4096).decode("UTF-8"))
            send_data = "Replay with:"
            connection.send(bytes(send_data, "UTF-8"))
        send_data = ""
        connection.close()


port = 9000


while True:

    server()

