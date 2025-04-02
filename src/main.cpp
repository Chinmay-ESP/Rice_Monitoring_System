#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>

#define SensorPin 34  // pH sensor connected to GPIO 34 (ADC1 channel)
#define ONE_WIRE_BUS 2  // Temperature sensor
#define SOIL_MOISTURE_PIN 32 // Soil Moisture Sensor

float calibration_value = 21.34 + 4;
unsigned long int avgValue;
int buf[50], temp;

// Temperature sensor setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// WiFi credentials
const char* ssid = "Pranali";
const char* password = "12345678";

// Google Apps Script Web App URL
const char* serverName = "https://script.google.com/macros/s/AKfycbzFqPLGDFIy7meWcvVnH7oKaLR72DBF4oak4aFu7EfFhouENrOf-N_sX3C6QTYm5huZig/exec";

// Initialize WiFi and NTP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000); // Update every 60 seconds

void setup() {
  Serial.begin(115200);
  pinMode(SensorPin, INPUT);
  sensors.begin(); // Initialize temperature sensor

  // Setup WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP client
  timeClient.begin();
  timeClient.update();
}

float readPH() {
  int i, j;
  for (i = 0; i < 50; i++) {  // Read 50 samples
    buf[i] = analogRead(SensorPin);
    delay(10);
  }

  // Sorting using Bubble Sort
  for (i = 0; i < 49; i++) {
    for (j = i + 1; j < 50; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  avgValue = 0;
  for (i = 15; i < 35; i++) avgValue += buf[i];  // Use middle 20 values for average

  float voltage = (float)avgValue * 3.3 / 4095.0 / 20.0; // Convert ADC reading to voltage
  float phValue = -5.70 * voltage + calibration_value; // Convert voltage to pH
  return phValue;
}

void loop() {
  // Read pH Sensor
  float phValue = readPH();

  // Read Soil Moisture Sensor
  int sensor_analog = analogRead(SOIL_MOISTURE_PIN);
  int _moisture = (100 - ((sensor_analog / 4095.00) * 100));
  float humidity = ((_moisture / 100.0) * 70) + 20;

  // Read Temperature Sensor
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
    return; // Skip sending invalid data
  }

  float tempF = sensors.toFahrenheit(tempC);

  // Improved Nutrient Calculations
  float n = (phValue * _moisture * tempC) / (humidity + 0.01);  // Avoid division by zero
  float p = (phValue * _moisture * humidity) / (tempC + 0.01);
  float k = (_moisture * tempC * humidity) / (phValue + 0.01);

  Serial.println("-----------------------------");
  Serial.print("Temp in C = "); Serial.println(tempC);
  Serial.print("Temp in F = "); Serial.println(tempF);
  Serial.print("Moisture = "); Serial.print(_moisture); Serial.println("%");
  Serial.print("Humidity = "); Serial.print(humidity); Serial.println("%");
  Serial.print("pH = "); Serial.println(phValue);
  Serial.print("N = "); Serial.println(n);
  Serial.print("P = "); Serial.println(p);
  Serial.print("K = "); Serial.println(k);

  if (WiFi.status() == WL_CONNECTED) { // Check if ESP32 is connected to WiFi
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Fetch time from NTP client
    timeClient.update();
    String formattedTime = timeClient.getFormattedTime();

    // Prepare JSON payload with sensor data
    String jsonData = "{\"method\":\"append\", \"temperature\":" + String(tempC) +
                      ", \"fahrenheit\":" + String(tempF) +
                      ", \"moisture\":" + String(_moisture) +
                      ", \"humidity\":" + String(humidity) +
                      ", \"ph\":" + String(phValue) +
                      ", \"n\":" + String(n) +
                      ", \"p\":" + String(p) +
                      ", \"k\":" + String(k) +
                      ", \"timestamp\":\"" + formattedTime + "\"}";

    // Send HTTP POST request
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response Code: " + String(httpResponseCode));
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }

    http.end(); // Close connection
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(1000); // Wait 1 second before next reading
}
