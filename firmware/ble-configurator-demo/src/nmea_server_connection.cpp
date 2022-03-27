#include "nmea_server_connection.hpp"
#include <WiFi.h>
#include <string>

IPAddress* serverIp = new IPAddress(192, 168, 0, 1);
uint32_t serverPort = 80;

class NmeaServerConnection NmeaServerConnection;

std::string NmeaServerConnection::getIp() {
    return serverIp->toString().c_str();
}

void NmeaServerConnection::setIp(std::string ip) {
    auto addr = new IPAddress();
    if (addr->fromString(ip.c_str())) {
        serverIp = addr;
    } else {
        // TODO: Log error here.
    }
}

uint32_t NmeaServerConnection::getPort() {
    return serverPort;
}

void NmeaServerConnection::setPort(uint32_t newPort) {
    serverPort = newPort;
}
