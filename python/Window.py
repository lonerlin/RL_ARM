#!usr/bin/python

from tkinter import *
from Arduino import *

tk=Tk()
tk.title('Arduino WIFI 扩展板 数字、模拟读写')

#IP地址和端口两个参数，需根据WIFI扩展板的实际IP和端口重新设置
arduino=Arduino("192.168.1.200",5000)

dv=StringVar()
av=StringVar()

def dwButtonClick():
    arduino.digitalWrite(dwPin.get(),dwValue.get())

def drButtonClick():
    dv.set(int(arduino.digitalRead(drPin.get())))

def awButtonClick():
    arduino.analogWrite(awPin.get(),awValue.get())

def arButtonClick():
    av.set(int(arduino.analogRead(arPin.get())))

def testButtonClick():
    arduino.sendMessage("Hello",9,1)

Label(tk,text='  针脚').grid(row=0,column=1,sticky=W,pady=3)
Label(tk,text='  值').grid(row=0,column=2,sticky=W)
Label(tk,text='  操作').grid(row=0,column=3,sticky=W)
Label(tk,text='写入数字值：').grid(row=1,sticky=W,pady=3)
Label(tk,text="读取数字值：").grid(row=2,sticky=W,pady=3)
Label(tk,text="写入模拟值：").grid(row=3,sticky=W,pady=3)
Label(tk,text="读取模拟值：").grid(row=4,sticky=W,pady=3)



dwPin=Spinbox(tk,from_=0, to=18,width=4)
drPin=Spinbox(tk,from_=0, to=18,width=4)
awPin=Spinbox(tk,from_=0, to=18,width=4)
arPin=Spinbox(tk,from_=0, to=5,width=4)
dwValue=Spinbox(tk,from_=0, to=1,width=6)
awValue=Spinbox(tk,from_=0, to=255,width=6)
drValue=Spinbox(tk,from_=0, to=1,state="readonly", textvariable=dv, width=6)
arValue=Spinbox(tk,from_=0, to=1024,state="readonly",textvariable=av, width=6)

Button(tk,text=" 写 入 ",command=dwButtonClick).grid(row=1,column=3,padx=5,pady=3)
Button(tk,text=" 读 取 ",command=drButtonClick).grid(row=2,column=3,padx=5,pady=3)
Button(tk,text=" 写 入 ",command=awButtonClick).grid(row=3,column=3,padx=5,pady=3)
Button(tk,text=" 读 取 ",command=arButtonClick).grid(row=4,column=3,padx=5,pady=3)

dwPin.grid(row=1,column=1,sticky=W,padx=5)
drPin.grid(row=2,column=1,sticky=W,padx=5)
awPin.grid(row=3,column=1,sticky=W,padx=5)
arPin.grid(row=4,column=1,sticky=W,padx=5)
dwValue.grid(row=1,column=2,sticky=W,padx=5)
awValue.grid(row=3,column=2,sticky=W,padx=5)
drValue.grid(row=2,column=2,sticky=W,padx=5)
arValue.grid(row=4,column=2,sticky=W,padx=5)


tk.mainloop()
