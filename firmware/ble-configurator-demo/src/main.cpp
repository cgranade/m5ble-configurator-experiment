#include <Arduino.h>
#include <M5EPD.h>

#define LGFX_M5PAPER
#include <LovyanGFX.hpp>

#include "configurator_service.hpp"
#include "wifi_connection.hpp"
#include "config_store.hpp"
#include "resources.hpp"

#include <sstream>

// TODO: Use sprites to accelerate drawing icons.
LGFX gfx;

int w;
int h;

#define SCREEN_HEIGHT 540
#define SCREEN_WIDTH 960
#define BOTTOM_MARGIN 10
#define LEFT_MARGIN 10
#define RIGHT_MARGIN 10
#define ICON_PADDING 8
#define ICON_ACTIVE TFT_BLACK
#define ICON_INACTIVE TFT_SILVER

// TODO: Don't update whole display every time – only redraw what we need.
void updateDisplay(void) {
    Serial.println("About to refresh display.");
    gfx.fillScreen(TFT_WHITE);
    gfx.setTextColor(TFT_BLACK, TFT_WHITE);
    gfx.setTextSize(1, 1);

    // TODO: Replace with actual speedometer display.
    gfx.setFont(&URW_Palladio_L_Bold_72);
    gfx.setTextDatum(textdatum_t::middle_centre);
    gfx.setTextSize(3.0);
    gfx.drawString("42.0", SCREEN_WIDTH / 2, (SCREEN_HEIGHT - FA_WIFI_HEIGHT) / 2);
    gfx.setTextSize(1.0);

    // Set font size to be appropriate for WiFi and battery status.
    gfx.setTextDatum(textdatum_t::middle_left);
    gfx.setFont(&fonts::FreeSerifBold12pt7b);

    // SD Card status
    gfx.drawBitmap(
      SCREEN_WIDTH - FA_SD_CARD_WIDTH - ICON_PADDING - FA_BLUETOOTH_B_WIDTH - RIGHT_MARGIN,
      SCREEN_HEIGHT - FA_SD_CARD_HEIGHT - BOTTOM_MARGIN,
      fa_sd_card,
      FA_SD_CARD_WIDTH, FA_SD_CARD_HEIGHT,
      SD.cardType() != CARD_NONE ? ICON_ACTIVE : ICON_INACTIVE
    );

    // BLE Status
    gfx.drawBitmap(
      SCREEN_WIDTH - FA_BLUETOOTH_B_WIDTH - RIGHT_MARGIN,
      SCREEN_HEIGHT - FA_BLUETOOTH_B_HEIGHT - BOTTOM_MARGIN,
      fa_bluetooth_b,
      FA_BLUETOOTH_B_WIDTH, FA_BLUETOOTH_B_HEIGHT,
      bleDeviceConnected ? ICON_ACTIVE : ICON_INACTIVE
    );

    // TODO: Draw battery status here.

    // Asking WiFi can take time, so give back control first.
    delay(1);
    // TODO: Consolidate code between branches.
    Serial.println("About to ask WiFi for status.");
    if (WiFiConnection.getStatus() == WL_CONNECTED) {
      gfx.drawBitmap(LEFT_MARGIN, SCREEN_HEIGHT - FA_WIFI_HEIGHT - BOTTOM_MARGIN, fa_wifi, FA_WIFI_WIDTH, FA_WIFI_HEIGHT, ICON_ACTIVE);
      std::ostringstream oss;
      gfx.drawString(WiFiConnection.getConnectedSsid().c_str(), LEFT_MARGIN + FA_WIFI_WIDTH + 8, SCREEN_HEIGHT - FA_WIFI_HEIGHT / 2 - BOTTOM_MARGIN / 2);
    } else {
      gfx.drawBitmap(LEFT_MARGIN, SCREEN_HEIGHT - FA_WIFI_HEIGHT - BOTTOM_MARGIN, fa_wifi, FA_WIFI_WIDTH, FA_WIFI_HEIGHT, ICON_INACTIVE);
    }

    // wifiIcon.pushSprite(100, 300);

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

  // wifiIcon.drawBitmap(0, 0, FA_WIFI_WIDTH, FA_WIFI_HEIGHT, fa_wifi);

  gfx.setAutoDisplay(false); 
  gfx.fillScreen(TFT_WHITE);
  // Update as soon as possible so that the user knows we're working.
  updateDisplay();

  // Make sure to load config data before trying to init communications.
  loadConfigToSdCard();

  initBLE();
  WiFiConnection.setup();
}

void loop(void) {
  M5.update();
  delay(5000);
  updateDisplay();
}
