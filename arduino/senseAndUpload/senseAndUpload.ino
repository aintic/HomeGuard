#include <WiFiManager.h>
#include <WiFi.h>
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"
#include "FirebaseConnection.h"
#include "pitches.h"
#include "time.h"

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

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

WiFiManager wifiManager;
DHT dht(DHT_PIN, DHT_TYPE);

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
long duration = 0;
float baseline = 0;

float temperatureC = 0;
float temperatureF = 0;
float humidity = 0;
float waterLevel = 0;
int smsCount = 0;
boolean statusNormal = false;
boolean floodNotification = false;
boolean recoveredNotification = false;
bool signupOK = false;

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

void playAlarm() {
  // iterate over the notes of the melody:
  // for (int thisNote = 0; thisNote < 8; thisNote++) {
  //   // to calculate the note duration, take one second divided by the note type.
  //   //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  //   int noteDuration = 1000 / noteDurations[thisNote];
  //   tone(BUZZER_PIN, melody[thisNote], noteDuration);

  //   // to distinguish the notes, set a minimum time between them.
  //   // the note's duration + 30% seems to work well:
  //   int pauseBetweenNotes = noteDuration * 1.30;
  //   delay(pauseBetweenNotes);
  //   // stop the tone playing:
  //   noTone(BUZZER_PIN);
  // }
  // myTone(BUZZER_PIN);
  // delay(20);
  // myNoTone(BUZZER_PIN);
  tone(BUZZER_PIN, NOTE_E3, 1000);
  delay(10);
  noTone(BUZZER_PIN);
}

// void myTone( int pin)
// {
//   ledcAttachPin(pin, 0);             // pin, channel
//   ledcWriteNote(0, NOTE_F, 4);    // channel, frequency, octave
// }

// void myNoTone( int pin)
// {
//   ledcDetachPin(pin);
// }

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

// The Firestore payload upload callback function
void fcsUploadCallback(CFS_UploadStatusInfo info)
{
    if (info.status == firebase_cfs_upload_status_init)
    {
        Serial.printf("\nUploading data (%d)...\n", info.size);
    }
    else if (info.status == firebase_cfs_upload_status_upload)
    {
        Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
    }
    else if (info.status == firebase_cfs_upload_status_complete)
    {
        Serial.println("Upload completed ");
    }
    else if (info.status == firebase_cfs_upload_status_process_response)
    {
        Serial.print("Processing the response... ");
    }
    else if (info.status == firebase_cfs_upload_status_error)
    {
        Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
    }
}

void sendSensorData() {
  if (Firebase.ready() && signupOK){
    // && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)
    // sendDataPrevMillis = millis();    
    // Write a Float number on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/temperatureC", temperatureC)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write a Float number on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/temperatureF", temperatureF)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write a Float number on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/humidity", humidity)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

        // Write a Float number on the database path sensor
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/waterLevel", waterLevel)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

void sendEventData() {
  if (Firebase.ready() && signupOK) {
    // Write a boolean on the database path event
    if (Firebase.RTDB.setBool(&fbdo, "event/floodNotification", floodNotification)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write a boolean on the database path event
    if (Firebase.RTDB.setBool(&fbdo, "event/recoveredNotification", recoveredNotification)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write a boolean on the database path event
    if (Firebase.RTDB.setBool(&fbdo, "event/statusNormal", statusNormal)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

void sendSMS() {
  FirebaseJson content;
  String documentPath = "sms/" + String(smsCount);
  content.set("fields/to/stringValue", PHONE);
  content.set("fields/body/stringValue", "Water detected in basement on 03/12/2023 2:27pm EST!");
  String doc_path = "projects/";
  doc_path += PROJECT_ID;
  doc_path += ("/databases/(default)/documents/sms/" + String(smsCount));
  // reference
  //content.set("fields/myRef/referenceValue", doc_path.c_str());
  smsCount++;
  Serial.print("Create a document... ");
  if (Firebase.Firestore.createDocument(&fbdo, PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  else
    Serial.println(fbdo.errorReason());
}

bool readAppData(String appData) {
  if (Firebase.ready() && signupOK) {
    if (appData.equals("floodNotificationSent")) {
      if (Firebase.RTDB.getInt(&fbdo, "app/floodNotificationSent")) {
        if (fbdo.dataType() == "boolean") {
          return fbdo.to<bool>();
        }
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    else if (appData.equals("recoveredNotificationSent")) {
      if (Firebase.RTDB.getInt(&fbdo, "app/recoveredNotificationSent")) {
        if (fbdo.dataType() == "boolean") {
          return fbdo.to<bool>();
        }
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
  }
}

void setup() {
  Serial.begin(115200); // Starts the serial communication

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
  Firebase.reconnectWiFi(true);
}

void loop() {
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperatureC = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  temperatureF = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  waterLevel = baseline - readFromUltrasonic();

  if (waterLevel < 0) {
    waterLevel = 0;
  }

  if (waterLevel < ACCURACY_OFFSET) {
    waterLevel = 0;
  }
  
  Serial.print("Water Level (cm): ");
  Serial.println(waterLevel);

  if (waterLevel != 0 && statusNormal) {
    statusNormal = false;
    floodNotification = true;
    playAlarm();
    sendEventData();
    if (smsCount < 1) {
      sendSMS();
    }
  }
  else if (waterLevel != 0 && statusNormal == false) {
    // check if notification was already sent
    // if not, send it again
    // bool floodNotificationSent = readAppData("floodNotificationSent");
    // if (floodNotification && floodNotificationSent) {
    //   floodNotification = false;
    // }
    playAlarm();
  }
  else if (waterLevel == 0 && statusNormal == false) {
    statusNormal = true;
    floodNotification = false;
    recoveredNotification = true;
    sendEventData();
  }
  else {
    statusNormal = true;
    floodNotification = false;
    recoveredNotification = false;
    // bool recoveredNotificationSent = readAppData("recoveredNotificationSent");
    // if (recoveredNotification && recoveredNotificationSent) {
    //   recoveredNotification = false;
    // }
  }

  signalLeds();
  sendSensorData();
}
