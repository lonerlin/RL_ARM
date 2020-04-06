
#include <WiFiSerial.h>
#include <Servo.h>

WiFiSerial wSerial(9600);
Servo s1,s2;

void MessageEvent(String order,int paraOne,int paraTwo)
{
    if(order == "servo"){
        if(paraOne>-1){
            s1.write(paraOne);
            Serial.print("s1 move angle:");
            Serial.println(paraOne);
          }
        if(paraTwo>-1){
          s2.write(paraTwo);
          Serial.print("s2 move angle:");
          Serial.println(paraTwo);
        }
      }
}

void setup()
{
  s1.attach(9);
  s2.attach(6);
  wSerial.Begin();
  wSerial.setMessageEvent(MessageEvent);
  
}

void loop()
{
   wSerial.SerialEvent();
  //Serial.print(dht11_pin_2.getTemperature());
  //Serial.println();
}
