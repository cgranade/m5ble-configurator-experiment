#pragma once
#include <string>

class NmeaServerConnection {
public:
    std::string getIp();
    void setIp(std::string ip);
    uint32_t getPort();
    void setPort(uint32_t port);
};

extern class NmeaServerConnection NmeaServerConnection;
