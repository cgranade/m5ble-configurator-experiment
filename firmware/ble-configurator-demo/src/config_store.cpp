#include "config_store.hpp"
#include "wifi_connection.hpp"
#include <ArduinoJson.h>
#include <SD.h>

#define CONFIG_PATH "/config.json"

void loadConfig() {
    if (!SD.exists(CONFIG_PATH)) {
        Serial.println("No config file on SD card, skipping load.");
        return;
    }

    Serial.println("Opening config...");
    auto file = SD.open(CONFIG_PATH);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);
    file.close();
    auto obj = doc.as<JsonObject>();

    Serial.println("Loading values from config...");
    WiFiConnection.setSsid(obj[F("ssid")]);
    WiFiConnection.setPass(obj[F("password")]);
}

void saveConfig() {
    Serial.println("Saving config...");
    DynamicJsonDocument doc(1024);
    doc[F("ssid")] = WiFiConnection.getSsid();
    doc[F("password")] = WiFiConnection.getPass();
    Serial.println("Opening file to write config into...");
    auto file = SD.open(CONFIG_PATH, "w");
    serializeJson(doc, file);
    file.close();
    Serial.println("Done saving config.");
}
