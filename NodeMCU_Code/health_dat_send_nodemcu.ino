#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define REPORTING_PERIOD_MS 10000  // send every 10 seconds to avoid API rate limit

const char* ssid = "Redmi";
const char* password = "123456789";

const char* thingspeakApiKey = "BEFUGECEV7HISV72";

PulseOximeter pox;
uint32_t tsLastReport = 0;

float heartRate, spo2;
float rr_intervals[10];
int rr_index = 0;
float lastBeat = 0;

// Respiration tracking
int breathCount = 0;
unsigned long lastBreathCheck = 0;
int respirationRate = 0;

void onBeatDetected() {
  float now = millis();
  float rr = now - lastBeat;
  lastBeat = now;

  if (rr_index < 10) {
    rr_intervals[rr_index++] = rr;
  } else {
    for (int i = 1; i < 10; i++) rr_intervals[i - 1] = rr_intervals[i];
    rr_intervals[9] = rr;
  }

  if ((now - lastBreathCheck) > 60000) {
    respirationRate = breathCount;
    breathCount = 0;
    lastBreathCheck = now;
  } else {
    breathCount++;
  }
}

float calcHRV() {
  if (rr_index < 2) return 0;
  float sum = 0, mean, sdnn = 0;
  for (int i = 0; i < rr_index; i++) sum += rr_intervals[i];
  mean = sum / rr_index;
  for (int i = 0; i < rr_index; i++) sdnn += pow(rr_intervals[i] - mean, 2);
  return sqrt(sdnn / rr_index);
}

float estimateSystolic(float heartRate, float spo2) {
  return 109 + 0.5 * heartRate + 0.1 * spo2;
}

float estimateDiastolic(float heartRate) {
  return 70 + 0.3 * heartRate;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  Serial.println("Initializing MAX30100...");
  if (!pox.begin()) {
    Serial.println("FAILED to initialize MAX30100");
    while (true);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
  Serial.println("MAX30100 initialized");
}

void loop() {
  pox.update();  // Must be called frequently

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    heartRate = pox.getHeartRate();
    spo2 = pox.getSpO2();

    if (heartRate >= 50 && heartRate <= 180 && spo2 >= 85 && spo2 <= 100) {
      Serial.print("Heart rate: "); Serial.print(heartRate); Serial.print(" bpm\t");
      Serial.print("SpO2: "); Serial.print(spo2); Serial.println(" %");

      float hrv = calcHRV();
      Serial.print("Estimated HRV (SDNN): "); Serial.print(hrv); Serial.println(" ms");

      int stressLevel = (hrv > 80) ? 1 : (hrv > 70) ? 2 : (hrv > 60) ? 3 :
                        (hrv > 50) ? 4 : (hrv > 40) ? 5 : (hrv > 30) ? 6 :
                        (hrv > 20) ? 7 : (hrv > 10) ? 8 : (hrv > 5) ? 9 : 10;

      Serial.print("Stress Level (1-10): "); Serial.println(stressLevel);
      Serial.print("Estimated Respiration Rate: ");
      Serial.print(respirationRate); Serial.println(" breaths/min");

      float systolic = estimateSystolic(heartRate, spo2);
      float diastolic = estimateDiastolic(heartRate);

      Serial.print("Estimated Systolic BP: "); Serial.println(systolic);
      Serial.print("Estimated Diastolic BP: "); Serial.println(diastolic);

      // Prepare ThingSpeak HTTP update URL
      String url = "http://api.thingspeak.com/update?api_key=" + String(thingspeakApiKey) +
                   "&field1=" + String(heartRate, 2) +
                   "&field2=" + String(spo2, 2) +
                   "&field3=" + String(hrv, 2) +
                   "&field4=" + String(stressLevel) +
                   "&field5=" + String(respirationRate) +
                   "&field6=" + String(systolic, 1) +
                   "&field7=" + String(diastolic, 1);

      Serial.print("Uploading to ThingSpeak... ");
      HTTPClient http;
      http.begin(url);

      int httpResponseCode = http.GET();
      if (httpResponseCode == 200) {
        Serial.println("Success!");
      } else {
        Serial.print("Failed. Response code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("Sensor data not in valid range. Skipping upload.");
    }
    Serial.println("-----------------------------------");
    tsLastReport = millis();
  }
}
