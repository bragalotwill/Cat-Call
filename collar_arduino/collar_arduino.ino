/*
 * CAT CALL PROJECT
 * 12/5/2021
 * 
 * This device is located on a cat. In addition to
 * measuring acceleration and sound level, it cycles
 * through connecting via BLE to four devices. Three
 * of these are fixed beacon points, from which RSSI
 * (signal strength values) are stored. The fourth is
 * where the acceleration, sound level, and RSSI
 * information is sent.
 */

#include <Wire.h>                 
#include "SparkFun_MMA8452Q.h"
#include "SoftwareSerial.h"
SoftwareSerial ble(4, 3); // TX, RX

MMA8452Q accel;      
int micPin = A0;

bool fakeRSSI = false;

             
void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600);
  Wire.begin();
  pinMode(micPin, INPUT);
  accel.init();  

  sendCommand("AT");
  // Master mode:
  sendCommand("AT+ROLE0");
  // Set this UUID:
  sendCommand("AT+CHARCADA");
  // Set this name
  sendCommand("AT+NAMECollar");
}

void sendCommand(const char * command) {
  //Serial.print("Command send :");
  //Serial.println(command);
  ble.println(command);
  //wait some time
  delay(100);

  char reply[100];
  int i = 0;
  while (ble.available()) {
    reply[i] = ble.read();
    i += 1;
    i %= 100;
  }
  //end the string
  reply[i] = '\0';
  //Serial.print(reply);
  //Serial.println("\nReply end");
  delay(100);
}

void updateRSSI(char beacon) {
  Serial.print("RSSI");
  Serial.print(beacon);
  Serial.print(":");
  if (fakeRSSI) {
    // Print fake RSSI value
    Serial.println(random(-150000, -30000)/1000.0, 5);
    return;
  }
    switch (beacon)
      {
        case 'A':
          sendCommand("AT+UUIDAAAA");
          break;
        case 'B':
          sendCommand("AT+UUIDBBBB");
          break;
        default:
        case 'C':
          sendCommand("AT+UUIDCCCC");
          break;
      }
    delay(10);
    // Send request for RSSI
    ble.write('r');
    // Give response time
    delay(100);
    if (ble.available()) {
      while(ble.available()){
        Serial.write(ble.read());
      }
      Serial.print("\n");
    }
    else { // No response from RSSI request
      Serial.println("unavailable");
    }
}

void updateCat() {
  //format data in csv style
  if (accel.available()) {
    Serial.print("Cat Data:");
    // accelerometer
    Serial.print(accel.getCalculatedX(), 5);
    Serial.print(",");
    Serial.print(accel.getCalculatedY(), 5);
    Serial.print(",");
    Serial.print(accel.getCalculatedZ(), 5);
    Serial.print(",");

    // mic amplitude
    Serial.println(analogRead(micPin));
  }
}

void loop() {
  updateRSSI('A');
  updateRSSI('B');
  updateRSSI('C');

  updateCat();
  
  delay(10);
}
