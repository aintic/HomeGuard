#include <WiFiManager.h>
#include <WiFi.h>
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"
#include "FirebaseConnection.h"
#include "pitches.h"

#define DHT_TYPE DHT11
#define DHT_PIN 13
#define RED_PIN 16
#define GREEN_PIN 17
#define BUZZER_PIN 21
#define SR04_ECHO_PIN 18
#define SR04_TRIG_PIN 5
#define SOUND_SPEED 0.034
#define ACCURACY_OFFSET 0.05

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// WifiManager object
WiFiManager wifiManager;

// Firebase variables
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

// DHT11 temp/humidity sensor object
DHT dht(DHT_PIN, DHT_TYPE);

// variables for sensor readings
long duration = 0;
float baseline = 0;
float temperatureC = 0;
float temperatureF = 0;
float humidity = 0;
float waterLevel = 0;

// variables for device states 
boolean statusNormal = true;
boolean floodNotification = false;
boolean recoveredNotification = false;
boolean buzzerOff = false;

// method to light LEDs according to device states
void signalLeds() {
  if (statusNormal) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
  }
  else {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
  }
}

// method to play buzzer
void playAlarm() {
  tone(BUZZER_PIN, NOTE_E3, 1000);
  delay(10);
  noTone(BUZZER_PIN);
}

// method to get water level reading from ultrasonic sensor
float readFromUltrasonic() {
  // Clears the trigPin
  digitalWrite(SR04_TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(SR04_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SR04_TRIG_PIN, LOW);

  duration = pulseIn(SR04_ECHO_PIN, HIGH);
  return duration * SOUND_SPEED/2;
}

// method to send temperature, humidity, water level data to realtime database
void sendSensorData() {
  if (Firebase.ready() && signupOK){  
    // Write temperature Celcius on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/temperatureC", temperatureC)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write temperature Farenheit on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/temperatureF", temperatureF)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write humidity on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/humidity", humidity)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }

        // Write water level on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/waterLevel", waterLevel)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

// method to send device states to realtime database
void sendEventData() {
  if (Firebase.ready() && signupOK) {
    // Write flood notification state on the database path event
    if (Firebase.RTDB.setBool(&fbdo, "event/floodNotification", floodNotification)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write recovered notification state on the database path event
    if (Firebase.RTDB.setBool(&fbdo, "event/recoveredNotification", recoveredNotification)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write normal notification state on the database path event
    if (Firebase.RTDB.setBool(&fbdo, "event/statusNormal", statusNormal)){
      Serial.print("PASSED. ");
      Serial.print("PATH: " + fbdo.dataPath() + ",");
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.print("FAILED. ");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

// method to send document to Firestore for SMS alert
void sendSMS(String smsType) {
  FirebaseJson content;
  String documentPath = "sms/";

  // set phone number
  content.set("fields/to/stringValue", PHONE);

  // set message body
  if (smsType.equals("flood")) {
    content.set("fields/body/stringValue", "Status Alert: Water detected in basement! Water level is " + String(waterLevel) + ". Check app for current water level.");
  }
  else if (smsType.equals("recovered")) {
    content.set("fields/body/stringValue", "Status Normal: Water no longer detected in basement!");
  }
  
  // send SMS message
  Serial.print("Create a document... ");
  if (Firebase.Firestore.createDocument(&fbdo, PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  else
    Serial.println(fbdo.errorReason());
}

// method to read buzzer toggle state from realtime database
bool readAppCommand() {
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.getBool(&fbdo, "app/buzzerOff")) {
      if (fbdo.dataType() == "boolean") {
        return fbdo.to<bool>();
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
  }
}

void setup() {
  Serial.begin(115200); // Starts the serial communication

  wifiManager.resetSettings();

  // setup password protected access point
  // go to 192.168.4.1 to setup wifi connection for device securely
  wifiManager.autoConnect("HomeGuardAP", "H0m3Guard");
  
  // set actuator pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // turn leds off
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  // start DTH11 sensor
  dht.begin();

  // setup HC-SR04 sensor pins
  pinMode(SR04_ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(DHT_PIN, INPUT);
  pinMode(SR04_TRIG_PIN, OUTPUT); // Sets the trigPin as an Output

  // read the baseline distance for initial dry condition
  baseline = readFromUltrasonic();
  Serial.print("Baseline (cm): ");
  Serial.println(baseline);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = FIREBASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);
}

void loop() {
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperatureC = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  temperatureF = dht.readTemperature(true);

  Serial.println("Temp: " + String(temperatureC) + ", humidity: " + String(humidity));

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // read distance and get water level
  waterLevel = baseline - readFromUltrasonic();

  // handle negative values
  if (waterLevel < 0) {
    waterLevel = 0;
  }

  // handle sensor accuracy
  if (waterLevel < ACCURACY_OFFSET)  {
    waterLevel = 0;
  }
  
  Serial.print("Water Level (cm): ");
  Serial.println(waterLevel);

  // check if user toggled buzzer off
  buzzerOff = readAppCommand();

  // water detected - leak/flood
  if (waterLevel != 0 && statusNormal) {
    // set device states
    statusNormal = false;
    floodNotification = true;

    // play alarm if user toggle buzzer on
    if (buzzerOff == false) {
      playAlarm();
    }

    // send sms alert
    sendEventData();
    sendSMS("flood");
  }

  // water detected - ongoing leak/flood
  else if (waterLevel != 0 && statusNormal == false) {
    // alert already sent 1st time
    floodNotification = false;

    // continue playing alarm if user toggled buzzer on
    if (buzzerOff == false) {
      playAlarm();
    }
  }

  // water not detected - leak/flood was addressed
  else if (waterLevel == 0 && statusNormal == false) {
    // set device states
    statusNormal = true;
    floodNotification = false;
    recoveredNotification = true;

    // send sms alert that things are back to normal
    sendEventData();
    sendSMS("recovered");
  }

  // water not detected - no leak/flood at all
  else {
    // set device states
    statusNormal = true;
    floodNotification = false;
    recoveredNotification = false;
    sendEventData();
  }

  // set LED lights according to device states
  signalLeds();

  // send all sensor data to Firebase
  sendSensorData();
}