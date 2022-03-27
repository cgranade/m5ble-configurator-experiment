#include "nmea_server_connection.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
#include <string>
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
    if (!httpClient.begin(serverHost.c_str(), serverPort, "/speed")) { return false; }

    auto responseCode = httpClient.GET();
    if (responseCode < 0) { return false; }
    // TODO: Check that it's in 200–299.
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, httpClient.getString());
    *speedAndHeading = std::make_pair(doc[F("sog")], doc[F("cog")]);
    return true;
}