#include "SoftwareSerial.h"
SoftwareSerial ble(4, 3); // TX, RX

void setup() {
  ble.begin(9600); // Bluetooth device
  Serial.begin(9600); // Computer debugging

  sendCommand("AT");
  sendCommand("AT+IMME1");
  sendCommand("AT+NOTI1");
  sendCommand("AT+NOTP1");
  sendCommand("AT+ROLE0");
  sendCommand("AT+NAMESlave");
  sendCommand("AT+FLAG0");
}

void reset() {
  sendCommand("AT");
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
  delay(1000);
}

void updateSerial() {
  // Output BLE data to computer:
  if (ble.available()) {
      char c = ble.read();
      if (c == '^') reset();
      Serial.write(c);
  }

  // Output computer data to BLE:
  if (Serial.available()) {
    ble.write(Serial.read());
  }
}

void loop() {
  updateSerial();
}
