#pragma once
#include <string>

class NmeaServerConnection {
public:
    std::string getHost();
    void setHost(std::string host);
    uint16_t getPort();
    void setPort(uint16_t port);

    bool getSpeedAndHeading(std::pair<double, double>* speedAndHeading);
};

extern class NmeaServerConnection NmeaServerConnection;
