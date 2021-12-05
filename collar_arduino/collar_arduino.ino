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
bool debug = true;

             
void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600);
  Wire.begin();
  pinMode(micPin, INPUT);
  accel.init();

  sendCommand("AT");
  sendCommand("AT+IMME1");
  sendCommand("AT+NOTI1");
  sendCommand("AT+NOTP1");
  sendCommand("AT+ROLE1");
}

String sendCommand(const char * command) {
  if(debug) Serial.print("Command send :");
  if(debug) Serial.println(command);
  ble.println(command);
  //wait some time
  delay(100);

  char reply[1000];
  int i = 0;
  while (ble.available()) {
    char newChar = ble.read();
    if (i < 998) {
    reply[i] = newChar;
    i += 1;
    }
  }
  //end the string
  reply[i] = '\0';
  if(debug) Serial.print(reply);
  if(debug) Serial.println("\nReply end");
  delay(1000);
  return (String) reply;
}

void updateRSSI() {
  // Get list of available devices
  String list = sendCommand("AT+DISC?");
  
  // Search in list for beacon A
  int loc = list.indexOf("209148593EF4");
  if (loc+24 < list.length() && debug) Serial.println("A in reply: \"" + list.substring(loc, loc + 24) + "\"");
  // Extract RSSI for beacon A
  // Search in list for beacon B
  // Extract RSSI for beacon B
  // Search in list for beacon C
  // Extract RSSI for beacon C
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

void updateSerial() {
  // Output BLE data to computer:
  if (ble.available()) {
      Serial.write(ble.read());
  }

  // Output computer data to BLE:
  if (Serial.available())
    ble.write(Serial.read());
}

void loop() {
  updateRSSI();

  updateCat();

  delay(10);

  // updateSerial();
}
