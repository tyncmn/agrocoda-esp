#include <WiFi.h>
#include <HTTPClient.h>
#include <DHTesp.h>

#define MOIS_PIN 34
#define GAS_PIN 35
#define TEMP_PIN 32
#define LIGHT_PIN 26
#define GREEN_PIN 33
#define BLUE_PIN 25

const char* ssid = "Pxmpkeen-Hotspot";
const char* password = "katletcorey";

// const char* ssid = "Holberton - Students";
// const char* password = "HBTNStuds24";

const char* serverUrl = "http://137.184.119.247:5533/data";
// const char* serverUrl = "https://api.qazaq.live/api/v1/telemetry/";
DHTesp dht;

void setup() {
  pinMode (GREEN_PIN, OUTPUT);
  pinMode (BLUE_PIN, OUTPUT);
  pinMode (LIGHT_PIN, INPUT);
  Serial.begin(9600);
  digitalWrite(GREEN_PIN, HIGH);
  connectToWiFi();
}

void loop() {
  float valMois = mapReverse(analogRead(MOIS_PIN), 0, 4096, 0, 100);
  float valGas = analogRead(GAS_PIN) * 0.24;
  // float valTemp = (analogRead(TEMP_PIN) * (3300.0/4096.0) ) / 10.0;
  dht.setup(TEMP_PIN, DHTesp::DHT11);
  TempAndHumidity vals = dht.getTempAndHumidity();
  float valTemp = vals.temperature;
  float valHum = vals.humidity;
  float valLight = analogRead(LIGHT_PIN);

  Serial.print("Moisture value: ");
  Serial.println(valMois);
  Serial.print("Gas value: ");
  Serial.println(valGas);
  Serial.print("Temperature value: ");
  Serial.println(valTemp);
  Serial.print("Humidity value: ");
  Serial.println(valHum);
  Serial.print("Light value: ");
  Serial.println(valLight);

  sendData(valMois, valGas, valTemp, valHum, valLight);

  delay(3000); // Send data every 5 seconds
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

//  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
//  }

  Serial.println("Connected to WiFi");
}

float mapReverse(float x, float in_min, float in_max, float out_min, float out_max) {
  return (out_max - out_min) * (in_max - x) / (in_max - in_min) + out_min;
}

void sendData(float moisture, float gas, float temperature, float humidity, float light) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Cannot send data.");
    digitalWrite(BLUE_PIN, LOW);
    return;
  }
  digitalWrite(BLUE_PIN, HIGH);
  HTTPClient http;

  int arduinoId = 123;
//  String data = "moisture=" + String(moisture) + "&temperature=" + String(temperature);
  String data = String("{\"arduino_id\":") + String(arduinoId) +
                ",\"temperature\":" + String(temperature, 2) +
                ",\"humidity\":" + String(humidity, 2) +
                ",\"gas\":" + String(gas, 2) +
                ",\"moisture\":" + String(moisture, 2) +
                "}";
              //    int httpResponseCode = http.POST(httpBody);

  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
