#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <DHT.h>

#define WIFI_SSID "realme"
#define WIFI_PASSWORD "12345678"
#define SERVER_URL "http://192.168.1.9:5000/data"  // Replace with your Flask server IP
#define REPORTING_PERIOD_MS 1000
#define DHT11PIN 4

PulseOximeter pox;
DHT dht(DHT11PIN, DHT11);
uint32_t tsLastReport = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    
    Serial.println("\nConnected to WiFi");
    
    if (!pox.begin()) {
        Serial.println("MAX30100 initialization FAILED");
        for(;;);
    }
    
    pox.setOnBeatDetectedCallback([]() { Serial.println("Beat!"); });
    
    dht.begin();
}

void loop() {
    pox.update();
    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        float heartRate = pox.getHeartRate();
        float spo2 = pox.getSpO2();
        float temperature = dht.readTemperature();

        Serial.print("Heart rate: "); Serial.print(heartRate); Serial.println(" bpm");
        Serial.print("SpO2: "); Serial.print(spo2); Serial.println(" %");
        Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");

        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(SERVER_URL);
            http.addHeader("Content-Type", "application/json");
            
            String jsonPayload = "{\"heart_rate\":" + String(heartRate) + 
                                 ", \"spo2\":" + String(spo2) + 
                                 ", \"temperature\":" + String(temperature) + "}";

            int httpResponseCode = http.POST(jsonPayload);
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            
            http.end();
        } else {
            Serial.println("WiFi Disconnected");
        }

        tsLastReport = millis();
    }
}
