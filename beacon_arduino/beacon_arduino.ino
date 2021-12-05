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

// Which of the three beacons is this?
#define ID 'A'

void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600); // Computer debugging

  sendCommand("AT");
  // Slave mode:
  sendCommand("AT+ROLE1");
  // Set server UUID:
  sendCommand("AT+UUIDCADA");
  switch (ID)
  {
    case 'A':
      // Set this UUID
      sendCommand("AT+CHARAAAA");
      // Set this name
      sendCommand("AT+NAMEBeaconA");
      break;
    case 'B':
      // Set this UUID
      sendCommand("AT+CHARBBBB");
      // Set this name
      sendCommand("AT+NAMEBeaconB");
      break;
    case 'C':
      // Set this UUID
      sendCommand("AT+CHARCCCC");
      // Set this name
      sendCommand("AT+NAMEBeaconC");
      break;
    default:
      break;
  }
  // Set to remote control mode:
  //sendCommand("AT+MODE2");
  
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
    reply[i] = ble.read();
    i += 1;
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
   // char input = ;
  /*  if (input == '}') {
      sendCommand("AT+RSSI?");
    }
    else {*/
      Serial.write(ble.read());
   // }
  }

  // Output computer data to BLE:
  if (Serial.available())
    ble.write(Serial.read());
}

void loop() {
  updateSerial();
}
