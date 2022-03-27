#include <Arduino.h>
#include <M5EPD.h>

#define LGFX_M5PAPER
#include <LovyanGFX.hpp>

#include "configurator_service.hpp"
#include "wifi_connection.hpp"
#include "config_store.hpp"

#include <sstream>

LGFX gfx;
LGFX_Sprite sp(&gfx);

int w;
int h;

void updateDisplay(void) {
    Serial.println("About to refresh display.");
    gfx.fillScreen(TFT_WHITE);
    gfx.setTextColor(TFT_BLACK, TFT_WHITE);
    gfx.setTextSize(1, 1);
    gfx.drawString(currentPrompt.c_str(), 10, 10);

    // SD Card status
    if (SD.cardType() == CARD_NONE) {
      gfx.drawString("SD card missing.", 10, 400);
    }

    // BLE Status
    if (bleDeviceConnected) {
      gfx.drawString("BLE: Connected.", 10, 60);
    } else {
      gfx.drawString("BLE: Disconnected.", 10, 60);
    }

    // Asking WiFi can take time, so give back control first.
    delay(1);
    Serial.println("About to ask WiFi for status.");
    if (WiFiConnection.getStatus() == WL_CONNECTED) {
      std::ostringstream oss;
      oss << "WiFi: Connected to " << WiFiConnection.getConnectedSsid() << ".";
      gfx.drawString(oss.str().c_str(), 10, 100);
    } else {
      gfx.drawString("WiFi: Disconnected.", 10, 100);
    }

    gfx.display();
    gfx.waitDisplay();

    // Make sure we don't refresh too often.
    delay(100);
}

void setup(void) {
  M5.begin();
  M5.RTC.begin();

  gfx.init();
  gfx.setRotation(1);

  gfx.setEpdMode(epd_mode_t::epd_quality);

  gfx.setAutoDisplay(false); 
  gfx.fillScreen(TFT_WHITE);
  gfx.setFont(&fonts::FreeSerifBold18pt7b);
  // Update as soon as possible so that the user knows we're working.
  updateDisplay();

  // Make sure to load config data before trying to init communications.
  loadConfig();

  initBLE();
  WiFiConnection.setup();
}

void loop(void) {
  M5.update();
  delay(5000);
  updateDisplay();
}
