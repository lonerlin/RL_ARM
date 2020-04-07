#!/usr/bin/env python3
"""Server for multithreaded (asynchronous) chat application."""
from socket import AF_INET, socket, SOCK_STREAM
from threading import Thread
import tkinter
from queue import Queue
from io import BytesIO
import time


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
        print("成功连接：{}".format(client_address))
        sd = Send(client, self.message)
        sd.start()

        # while True:
        #     ard_msg = client.recv(self.buf_size).decode("utf8")
        #     print(ard_msg)
        #     msg_list.insert(tkinter.END, ard_msg)
        #     msg_list.see(tkinter.END)

        with BytesIO() as buffer:
            while True:
                try:
                    resp = client.recv(1024)  # Read in some number of bytes -- balance this
                except BlockingIOError:
                    print("sleeping")  # Do whatever you want here, this just
                    time.sleep(2)  # illustrates that it's nonblocking
                else:
                    buffer.write(resp)  # Write to the BytesIO object
                    buffer.seek(0)  # Set the file pointer to the SoF
                    start_index = 0  # Count the number of characters processed
                    for line in buffer:
                        start_index += len(line)
                        #handle_line(line)  # Do something with your line
                        print(line)
                        msg_list.insert(tkinter.END, line)
                        msg_list.see(tkinter.END)


                    """ If we received any newline-terminated lines, this will be nonzero.
                        In that case, we read the remaining bytes into memory, truncate
                        the BytesIO object, reset the file pointer and re-write the
                        remaining bytes back into it.  This will advance the file pointer
                        appropriately.  If start_index is zero, the buffer doesn't contain
                        any newline-terminated lines, so we set the file pointer to the
                        end of the file to not overwrite bytes.
                    """
                    if start_index:
                        buffer.seek(start_index)
                        remaining = buffer.read()
                        buffer.truncate(0)
                        buffer.seek(0)
                        buffer.write(remaining)
                    else:
                        buffer.seek(0, 2)


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


