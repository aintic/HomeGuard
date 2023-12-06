#include "BluetoothSerial.h"
#include <ESP32Servo.h>

#define SERVO_PIN 12

Servo waterValve;
BluetoothSerial ESP_BT;

int incoming;

void setup() {
  Serial.begin(9600);

  waterValve.attach(SERVO_PIN);
  waterValve.write(0);

  ESP_BT.begin("Main Water Valve Control"); //Name of Bluetooth Signal
  Serial.println("Bluetooth Device is Ready to Pair");
}

void loop() {
  if (ESP_BT.available()) /*Check if we receive anything from Bluetooth*/ {
    incoming = ESP_BT.read(); //Read what we recevive 
    Serial.print("Received:"); 
    Serial.println(incoming);

    // signal to turn valve off
    if (incoming == 0) {
      waterValve.write(90);
      Serial.println("Main water valve turned off!");
      ESP_BT.println("Off");
    }

    // signal to turn valve on
    else if (incoming == 1) {
      waterValve.write(-90);
      Serial.println("Main water valve turned on!");
      ESP_BT.println("On");
    }
  }
  delay(20);
}