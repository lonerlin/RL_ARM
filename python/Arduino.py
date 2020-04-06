

class Arduino:
    '''
    所有消息的格式是：H,order(String),paraOne(int),paraTwo(int),
    digtialWrite ,digitalRead,analogWrite,analogRead四个函数，实现了对Arduino针脚的透明读写
    '''

    #指定Arduino WiFi扩展板的IP地址和端口
    def __init__(self, que_message):

        self.message = que_message

    def digitalWrite(self, pin, value):
        self.message.put("H,$dw, " + str(pin) + "," + str(value) + ", ")
        return None
    def digitalRead(self,pin):
       pass
       # return self.arduino.send("H,$dr," + str(pin) + ",0,").decode('utf-8').split(',')[3]

    def analogWrite(self, pin, value):
        self.message.add("H,$aw, " + str(pin) + "," + str(value) + ", ")
        return None

    def analogRead(self, pin):
        pass
        #value = self.arduino.send("H,$ar," + str(pin) + ",0,").decode('utf-8').split(',')
        #return value[3]

    #向Arduino发送自定义消息。
    def sendMessage(self, Order, ParaOne, ParaTwo):
        self.message.put("H," + Order + "," + str(ParaOne) + "," + str(ParaTwo) + ",").decode('utf-8').split(',')
