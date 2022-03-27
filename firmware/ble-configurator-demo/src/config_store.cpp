#include "config_store.hpp"
#include "wifi_connection.hpp"
#include "nmea_server_connection.hpp"
#include <ArduinoJson.h>
#include <SD.h>

#define CONFIG_PATH "/config.json"

DynamicJsonDocument configAsDoc() {
    DynamicJsonDocument doc(1024);
    doc[F("ssid")] = WiFiConnection.getSsid();
    doc[F("password")] = WiFiConnection.getPass();
    doc[F("ip")] = NmeaServerConnection.getIp();
    doc[F("port")] = NmeaServerConnection.getPort();
    return doc;
}

void applyConfig(JsonObject obj) {
    Serial.println("Loading values from config...");
    WiFiConnection.setSsid(obj[F("ssid")]);
    WiFiConnection.setPass(obj[F("password")]);
    NmeaServerConnection.setIp(obj[F("ip")]);
    NmeaServerConnection.setPort(obj[F("port")]);
}

void loadConfigToSdCard() {
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
    applyConfig(obj);
}

void loadConfigFromString(std::string json) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    applyConfig(doc.as<JsonObject>());
}

void saveConfigFromSdCard() {
    Serial.println("Saving config...");
    DynamicJsonDocument doc = configAsDoc();
    Serial.println("Opening file to write config into...");
    auto file = SD.open(CONFIG_PATH, "w");
    serializeJson(doc, file);
    file.close();
    Serial.println("Done saving config.");
}

std::string saveConfigToString() {
    char json[1024];
    serializeJson(configAsDoc(), json);
    return json;
}
