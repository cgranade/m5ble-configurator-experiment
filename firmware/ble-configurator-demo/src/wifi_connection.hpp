#pragma once
#include <WiFi.h>
#include <string>

class WiFiConnection {
public:
    WiFiConnection();
    ~WiFiConnection() = default;
    bool setup(void);
    bool teardown(void);

    std::string getConnectedSsid();

    // NB: Set methods have no effect until you reconnect!
    std::string getSsid();
    void setSsid(std::string ssid);
    std::string getPass();
    void setPass(std::string pass);

    wl_status_t getStatus();
};

extern class WiFiConnection WiFiConnection;
