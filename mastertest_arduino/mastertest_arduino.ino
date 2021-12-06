#include "SoftwareSerial.h"
SoftwareSerial ble(4,3); // TX, RX
int BLEpow = 5;

void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600); // Computer debugging
  pinMode(BLEpow, OUTPUT);
  digitalWrite(BLEpow, HIGH);

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
  delay(1000);
}

void updateSerial() {
  // Output BLE data to computer:
  if (ble.available()) {
      Serial.write(ble.read());
  }

  // Output computer data to BLE:
  if (Serial.available()) {
    char c = Serial.read();
    ble.write(c);
    if (c == '^') { // Disconnect
      // Power cycle
      digitalWrite(BLEpow, LOW);
      delay(500);
      digitalWrite(BLEpow, HIGH);
      bleInit();
    }
  }
}

void loop() {
  updateSerial();
}
