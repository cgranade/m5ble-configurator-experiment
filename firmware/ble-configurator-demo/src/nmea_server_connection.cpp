#include "nmea_server_connection.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
#include <string>
#include <sstream>
#include <ArduinoJson.h>

std::string serverHost = "192.168.0.1";
uint16_t serverPort = 80;

WiFiClient wifiClient;

class NmeaServerConnection NmeaServerConnection;

std::string NmeaServerConnection::getHost() {
    return serverHost;
}

void NmeaServerConnection::setHost(std::string newHost) {
    serverHost = newHost;
}

uint16_t NmeaServerConnection::getPort() {
    return serverPort;
}

void NmeaServerConnection::setPort(uint16_t newPort) {
    serverPort = newPort;
}

bool NmeaServerConnection::getSpeedAndHeading(std::pair<double, double>* speedAndHeading) {
    HTTPClient httpClient;
    std::ostringstream oss;
    oss << "http://" << serverHost << ":" << serverPort << "/speed";
    String url = oss.str().c_str();
    Serial.printf("Requesting SOG/COG from %s.\n", url.c_str());
    if (!httpClient.begin(url)) {
        Serial.println("HTTP client failed.");
        return false;
    }

    auto responseCode = httpClient.GET();
    Serial.printf("Request returned response code %i.\n", responseCode);
    if (responseCode < 0) {
        return false;
    }
    // TODO: Check that it's in 200–299.
    auto resp = httpClient.getString();
    Serial.printf("Got response: %s\n", resp.c_str());
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, resp);
    *speedAndHeading = std::make_pair(doc[F("sog")], doc[F("cog")]);
    return true;
}
