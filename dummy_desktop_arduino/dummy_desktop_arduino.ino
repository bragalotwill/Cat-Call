#include <Wire.h>                 
#include "SparkFun_MMA8452Q.h"    

MMA8452Q accel;      
int micPin = A0;

             
void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  pinMode(micPin, INPUT);
  accel.init();  
}


void loop() {

  //format data in csv style
  if (accel.available()) {   
    
    //fake rssi values
    Serial.print(random(-150000, -30000)/1000.0, 5);
    Serial.print(",");
    Serial.print(random(-150000, -30000)/1000.0, 5);
    Serial.print(",");
    Serial.print(random(-150000, -30000)/1000.0, 5);
    Serial.print(",");

    //accelerometer
    Serial.print(accel.getCalculatedX(), 5);
    Serial.print(",");
    Serial.print(accel.getCalculatedY(), 5);
    Serial.print(",");
    Serial.print(accel.getCalculatedZ(), 5);
    Serial.print(",");

    //mic amplitude
    Serial.println(analogRead(micPin));
  }
  
  delay(10);
}
