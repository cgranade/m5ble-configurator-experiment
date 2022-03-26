#include "wifi_connection.hpp"
#include "main.hpp"
#include <WiFi.h>

std::string *ssid = new std::string("<unset>");
std::string *pass = new std::string("<unset>");

class WiFiConnection WiFiConnection;

WiFiConnection::WiFiConnection() {
    WiFiProvEventCb callback = [](system_event_t *sys_event, wifi_prov_event_t *prov_event) {
        Serial.println("WiFi ETH or AP disconnect fired.");
    };
    WiFi.onEvent(callback, SYSTEM_EVENT_ETH_DISCONNECTED);
    WiFi.onEvent(callback, SYSTEM_EVENT_AP_STADISCONNECTED);
}

bool WiFiConnection::setup() {
    Serial.printf("Trying to connect to WiFi with ssid %s and password %s.", ssid->c_str(), pass->c_str());
    WiFi.begin(ssid->c_str(), pass->c_str());

    // Wait some time to connect to wifi
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // Check if connected to wifi
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("No WiFi!");
        return false;
    }

    Serial.println("Connected to WiFi, Connecting to server.");
    return true;
}

bool WiFiConnection::teardown() {
    return WiFi.disconnect();
}

wl_status_t WiFiConnection::getStatus() {
    return WiFi.status();
}

std::string WiFiConnection::getConnectedSsid() {
    auto ssid = WiFi.SSID();
    return ssid.c_str();
}

std::string WiFiConnection::getSsid() {
    return *ssid;
}
void WiFiConnection::setSsid(std::string newSsid) {
    *ssid = newSsid;
    Serial.printf("Set SSID config to %s.", ssid->c_str());
}
std::string WiFiConnection::getPass() {
    return *pass;
}
void WiFiConnection::setPass(std::string newPass) {
    *pass = newPass;
    Serial.printf("Set password config to %s.", pass->c_str());
}
