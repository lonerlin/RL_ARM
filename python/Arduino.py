from clientsocket import *

class Arduino:
    '''
    所有消息的格式是：H,order(String),paraOne(int),paraTwo(int),
    digtialWrite ,digitalRead,analogWrite,analogRead四个函数，实现了对Arduino针脚的透明读写
    '''

    #指定Arduino WiFi扩展板的IP地址和端口
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.arduino = ClientSocket(ip, port, single_use=False)

    def digitalWrite(self, pin, value):
        self.arduino.send("H,$dw, " + str(pin) + "," + str(value) + ", ")
        return None

    def digitalRead(self, pin):
        return self.arduino.send("H,$dr," + str(pin) + ",0,").decode('utf-8').split(',')[3]

    def analogWrite(self, pin, value):
       self.arduino.send("H,$aw, " + str(pin) + "," + str(value) + ", ")
       return None

    def analogRead(self, pin):
        value = self.arduino.send("H,$ar," + str(pin) + ",0,").decode('utf-8').split(',')
        return value[3]

    #向Arduino发送自定义消息。
    def sendMessage(self, Order, ParaOne, ParaTwo):
        return self.arduino.send("H," + Order + "," + str(ParaOne) + "," + str(ParaTwo) + ",").decode('utf-8').split(',')
