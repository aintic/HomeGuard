#include <WiFiManager.h>

WiFiManager wifiManager;

void setup() {
  // setup password protected access point at 192.168.4.1
  wifiManager.autoConnect("AutoConnectAP", "H0m3GuardSyst3m");
}

void loop() {
  
}
