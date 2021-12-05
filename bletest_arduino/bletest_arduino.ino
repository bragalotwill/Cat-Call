/*
   CAT CALL PROJECT
   12/5/2021

   This device provides a fixed point for comparison
   with a cat collar's location. All it does is wait
   for a BLE connection to be occasionally initiated by
   collar.
*/

#include "SoftwareSerial.h"
SoftwareSerial ble(4, 3); // TX, RX

/*
   Beacon A UUID: AAAA
   Beacon B UUID: BBBB
   Beacon C UUID: CCCC
   Collar UUID:   CADA
   Desktop UUID:  DECD
*/

void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600); // Computer debugging

  sendCommand("AT");
  // Master mode:
  sendCommand("AT+ROLE1");
  // Set server UUID:
  sendCommand("AT+UUIDAAAA");
  // Set this UUID
  sendCommand("AT+CHARCADA");
  // Set this name
  sendCommand("AT+NAMEMaster");
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
  if (Serial.available())
    ble.write(Serial.read());
}

void loop() {
  updateSerial();
}
