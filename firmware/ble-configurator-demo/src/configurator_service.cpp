#include "configurator_service.hpp"
#include "wifi_connection.hpp"
#include "nmea_server_connection.hpp"
#include "config_store.hpp"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "57bf47c8-8bf6-489f-b89b-109e9f892602"
#define WIFI_RECONNECT_CHARACTERISTIC_UUID "92996f42-419a-4162-8b70-96aa7a9d891e"
#define CONFIG_JSON_CHARACTERISTIC_UUID "7c087fc6-5a7c-4aac-8cc2-ffc11799eb27"
#define SAVE_CONFIG_CHARACTERISTIC_UUID "62c4e57d-e7df-4eec-884b-13587fa2f0bf"

BLEServer *bleServer;
BLEService *configuratorService;

bool bleDeviceConnected = false;

class ConfiguratorServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* server) {
    Serial.println("BLE server connected to central.");
    bleDeviceConnected = true;
    updateDisplay();
    delay(1);
  }

  void onDisconnect(BLEServer* server) {
    Serial.println("BLE server disconnected from central.");
    bleDeviceConnected = false;
    BLEAdvertising *advertising = bleServer->getAdvertising();
    advertising->start();
    updateDisplay();
    delay(1);
  }
};

using getter_cb_t = auto (*)(void) -> std::string;
using setter_cb_t = auto (*)(std::string) -> void;

class PropertyCharacteristicCallbacks: public BLECharacteristicCallbacks {
  public:
    PropertyCharacteristicCallbacks(getter_cb_t getter, setter_cb_t setter) :
      getter(getter), setter(setter) {};

    void onRead(BLECharacteristic *characteristic) {
      Serial.println("Reading property.");
      characteristic->setValue(this->getter());
      delay(1);
    }

    void onWrite(BLECharacteristic *characteristic) {
      Serial.println("Writing property.");
      this->setter(characteristic->getValue());
      delay(1);
    }

  private:
    getter_cb_t getter;
    setter_cb_t setter;
};

void createCharacteristicForProperty(BLEService* service, const char* uuid, getter_cb_t getter, setter_cb_t setter) {
  auto characteristic = service->createCharacteristic(
    uuid,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE
  );
  characteristic->setCallbacks(new PropertyCharacteristicCallbacks(getter, setter));
}

class WiFiReconnectCharacteristicCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    // Don't look at the value — any write means to disconnect and reconnect.
    Serial.println("Got command to reconnect to WiFi.");
    WiFiConnection.teardown();
    // Let everything settle down after disconnecting.
    delay(500);
    WiFiConnection.setup();
    updateDisplay();
  }
};

class SaveConfigCharacteristicCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    // Don't look at the value — any write means to same out the config.
    Serial.println("Got command to save config to SD card.");
    saveConfigFromSdCard();
  }
};

void initBLE() {
  BLEDevice::init("m5paper");
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new ConfiguratorServerCallbacks());
  configuratorService = bleServer->createService(SERVICE_UUID);

  createCharacteristicForProperty(
    configuratorService, CONFIG_JSON_CHARACTERISTIC_UUID,
    [] () {
      return saveConfigToString();
    },
    [] (std::string newConfig) {
      loadConfigFromString(newConfig);
    }
  );

  auto reconnectCharacteristic = configuratorService->createCharacteristic(
    WIFI_RECONNECT_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  reconnectCharacteristic->setCallbacks(new WiFiReconnectCharacteristicCallback());

  auto saveConfigCharacteristic = configuratorService->createCharacteristic(
    SAVE_CONFIG_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  saveConfigCharacteristic->setCallbacks(new SaveConfigCharacteristicCallback());

  configuratorService->start();
  BLEAdvertising *advertising = bleServer->getAdvertising();
  advertising->start();
}
