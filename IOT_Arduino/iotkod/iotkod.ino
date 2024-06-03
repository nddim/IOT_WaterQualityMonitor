//biblioteke
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HCSR04.h>
#include <IOXhop_FirebaseESP32.h>
#include <ESP32Servo.h>

//firebase
#define FIREBASE_HOST "your_firebase_host"
#define FIREBASE_AUTH "your_firebase_auth"
#define FIREBASE_projectID "your_reference_url"

//wifi
const char* ssid = "your_wifi_id";
const char* password = "your_wifi_pw";


//HCSR04 sensor udaljenosti
float dist = 0;
HCSR04 hc(32, 33);  //initialisation class HCSR04 (trig pin , echo pin)

// Data wire is conntected to the  34
#define ONE_WIRE_BUS 25
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
float temp = 0;

// servo motor
Servo servo;
int servoPin = 18;
void setup() {
  Serial.begin(9600);

  //wifi postavke
  delay(1000);
  WiFi.mode(WIFI_STA);  //Optional
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WIFI…");

  sensors.begin();
  int counter = 0;

  while (WiFi.status() != WL_CONNECTED) {
    counter++;
    delay(250);
    Serial.print(".");
    if (counter > 50) {
      Serial.println("Restartuje se esp!");
      ESP.restart();
    }
  }
  Serial.println("Uspjesno povezan");

  //firebase postavke
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  servo.attach(18);
  servo.write(45);

  delay(500);
}

void loop() {
  //sensozr udaljenosti

  dist = 19.2 - hc.dist();
  Serial.println(dist);  //return current distance (cm) in serial

  Firebase.setFloat("udaljenost", dist);

  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.print("Celsius temperature: ");

  Serial.print(sensors.getTempCByIndex(0));
  Firebase.setFloat("temperatura", temp);
  servo.write(0);

  // Get the boolean value from Firebase
  bool boolValue = Firebase.getBool("bool");

  if (boolValue) {
    If bool is true, continuously rotate the servo from 0 to 150 degrees
                       servo.write(0);
    delay(100);
    servo.write(180);

    Firebase.setBool("bool", false);
    boolValue = false;
  }
  if (boolValue == false) {
    // If bool is false, rotate the servo to 0 degrees
    servo.write(0);
    delay(100);  // You can adjust this delay as needed
  }

  delay(100);  // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.

  servo.write(0);
}