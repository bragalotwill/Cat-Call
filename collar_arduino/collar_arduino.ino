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
int BLEpow = 5;

MMA8452Q accel;      
int micPin = A0;

bool fakeRSSI = false;
bool debug = true;

long period;
int desktopFails;

int BLEpow = 5;
             
void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600);
  Wire.begin();
  pinMode(micPin, INPUT);
  accel.init();

  period = 10;
  desktopFails = 0;

  bleInit();
}

void bleInit() {
  sendCommand("AT");
  sendCommand("AT+IMME1");
  sendCommand("AT+NOTI1");
  sendCommand("AT+NOTP1");
  sendCommand("AT+ROLE1");
  sendCommand("AT+NAMEMaster");
}

char* sendCommand(const char * command) {
  if(debug) Serial.print("Command send :");
  if(debug) Serial.println(command);
  ble.println(command);
  //wait some time
  delay(100);

  static char reply[100];
  int i = 0;
  while (ble.available()) {
    char newChar = ble.read();
    if (i < 98) {
    reply[i] = newChar;
    i += 1;
    }
  }
  //end the string
  reply[i] = '\0';
  if(debug) Serial.print(reply);
  if(debug) Serial.println("\nReply end ");
  delay(1000);
  return reply;
}

bool connDesktop() {
 // Get list of available devices
 String list = sendCommand("AT+DISC?");
 // Find index of computer arduino, if present
  int locID = list.indexOf("B0B1136840A0");
  int locDIS = list.indexOf("DIS", locID - 6);
  bool fail = false;
  if (locID > 0 && locDIS > 0) {
    String indexStr = list.substring(locDIS + 3, locID-1);
    int len = indexStr.length();
    int index = 0;
    for (int dig = 0; dig < len; dig++) index += (indexStr.charAt(dig) - '0') * pow(10, dig);
    if (debug) Serial.print("Index of Computer is ");
    if (debug) Serial.println(index);
    String command = "AT+CONN" + indexStr;
    String attempt = sendCommand(command.c_str());
  } else fail = true;

  if (fail) desktopFails++;
  else desktopFails = 0;
}

void sendDesktop(String data) {
  if (connDesktop()) {
    ble.println(data);
    //wait some time
    delay(100);
  }

  // Terminate connection
  sendCommand("^");
  sendCommand("AT");
  Serial.println("Attempting to disconnect...");
  // Power cycle
  digitalWrite(BLEpow, LOW);
  delay(500);
  digitalWrite(BLEpow, HIGH);
  delay(100);
  digitalWrite(BLEpow, LOW);
  delay(1000);
  digitalWrite(BLEpow, HIGH);
  bleInit();
}

void updateRSSI() {
  // Get list of available devices
  String list = sendCommand("AT+DISC?");

  // Beacon A
  findRSSI(list, "209148593EF4", "A");
  // Beacon B
  findRSSI(list, "20914840620F", "B");
  // Beacon C
  findRSSI(list, "D0B5C295600D", "C");
}

void findRSSI(String list, String address, String beacon) {
  // Search in list for beacon 
  int locID = list.indexOf(address);
  // Extract RSSI for beacon 
  int locRSSI = list.indexOf("RSSI:", locID);
  String rssi = "";
  if (locID > 0 && locRSSI > 0 && locRSSI + 9 < list.length()) {
    rssi = list.substring(locRSSI + 4, locRSSI + 9);
  } else {
    rssi = "unavailable";
  }

  String data = "RSSI" + beacon + ":" + rssi;
  if (debug) Serial.println(data);
  sendDesktop(data);
}

void updateCat() {
  //format data in csv style
  String data = "";
  if (accel.available()) {
    data += "Cat Data:";
    // accelerometer
  /*  char buffer[6];
    data += dtostrf(accel.getCalculatedX(), 5, 3, buffer);
    data += ",";
    data += dtostrf(accel.getCalculatedY(), 5, 3, buffer);
    data += ",";
    data += dtostrf(accel.getCalculatedZ(), 5, 3, buffer);
    data += ",";*/

    // mic amplitude
    data += analogRead(micPin);

    sendDesktop(data);
  }
}

void checkDeskConnection() {
  if (desktopFails > 20) {
    period *= 10;
    if (debug) Serial.println("Desktop hasn't responded in last 20 connection attempts.");
  }
  // Max delay time of 17 minutes
  if (period >= 1000000) {
    period = 1000000;
  }
}

void loop() {
  updateRSSI();

  updateCat();

  delay(period);

  checkDeskConnection();

  // updateSerial();
}
