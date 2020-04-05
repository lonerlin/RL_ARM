#!/usr/bin/env python3
"""Server for multithreaded (asynchronous) chat application."""
from socket import AF_INET, socket, SOCK_STREAM
from threading import Thread
import tkinter
from queue import Queue

class Server(Thread):
    def __init__(self, que_message, host='192.168.0.174', port=9000, buf_size=1024):
        Thread.__init__(self)
        self.message = que_message
        self.sever = socket(AF_INET, SOCK_STREAM)
        self.sever.bind((host, port))
        self.buf_size = buf_size

    def run(self):
        self.sever.listen(1)
        client, client_address = self.sever.accept()
        sd = Send(client, self.message)
        sd.start()
        while True:
            ard_msg = client.recv(self.buf_size).decode("utf8")
            msg_list.insert(tkinter.END, ard_msg)


class Send(Thread):
    def __init__(self, client, que_message):
        Thread.__init__(self)
        self.client = client
        self.message = que_message

    def run(self):
        while True:
            if not self.message.empty():
                msg = self.message.get(False)
                print("msg: {}".format(msg))
                self.client.send(bytes(msg, "utf8"))


def send(event=None):  # event is passed by binders.
    """Handles sending of messages."""
    msg = my_msg.get()
    print("tk msg:{}".format(msg))
    my_msg.set("")  # Clears input field.
    messages_list.put_nowait(msg)
    if msg == "{quit}":
        top.quit()

def on_closing(event=None):
    """This function is to be called when the window is closed."""
    my_msg.set("{quit}")
    send()
    tkinter._exit()


top = tkinter.Tk()
top.title("WIFI串口无线通信")

messages_frame = tkinter.Frame(top)
my_msg = tkinter.StringVar()  # For the messages to be sent.
my_msg.set("")
scrollbar = tkinter.Scrollbar(messages_frame)  # To navigate through past messages.
# Following will contain the messages.
msg_list = tkinter.Listbox(messages_frame, height=15, width=80, yscrollcommand=scrollbar.set)
scrollbar.pack(side=tkinter.RIGHT, fill=tkinter.Y)
msg_list.pack(side=tkinter.LEFT, fill=tkinter.BOTH)
msg_list.pack()
messages_frame.pack()

entry_field = tkinter.Entry(top, textvariable=my_msg, width=80)
entry_field.bind("<Return>", send)
entry_field.pack()
send_button = tkinter.Button(top, text=" 发  送 ", command=send)
send_button.pack()

top.protocol("WM_DELETE_WINDOW", on_closing)
messages_list = Queue(0)
ser = Server(messages_list)
ser.start()
tkinter.mainloop()  # Starts GUI execution.


