/*
 * CAT CALL PROJECT
 * 12/5/2021
 * 
 * This device recieves acceleration, RSS, and sound information
 * from the cat collar via a BLE connection. Then, it provides the
 * information over a serial port.
 */
 
#include "SoftwareSerial.h"
SoftwareSerial ble(4, 3); // TX, RX
int BLEpow = 5;

void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600); // Computer debugging

  sendCommand("AT+IMME1");
  sendCommand("AT+NOTI1");
  sendCommand("AT+NOTP1");
  sendCommand("AT+ROLE0");
  sendCommand("AT+NAMEComputer");
  sendCommand("AT+FLAG0");
}

void sendCommand(const char * command) {
  Serial.print("Command send :");
  Serial.println(command);
  ble.println(command);
  //wait some time
  delay(100);

  char reply[100];
  int i = 0;
  while (ble.available()) {
    char newChar = ble.read();
    if (i < 99) {
    reply[i] = newChar;
    i += 1;
    }
  }
  //end the string
  reply[i] = '\0';
  Serial.print(reply);
  Serial.println("\nReply end");
  delay(100);
}

void updateSerial() {
  // Output BLE data to computer:
  if (ble.available()) {
      Serial.write(ble.read());
  }

  // Output computer data to BLE:
  if (Serial.available()) {
    ble.write(Serial.read());
  }
}

void loop() {
  updateSerial();
}
