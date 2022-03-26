#include "configurator_service.hpp"
#include "wifi_connection.hpp"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "57bf47c8-8bf6-489f-b89b-109e9f892602"
#define PROMPT_CHARACTERISTIC_UUID "76883ff7-01dc-4750-9659-f5336e81764a"
#define WIFI_SSID_CHARACTERISTIC_UUID "22834252-e71c-4b13-a050-8951874c5b20"
#define WIFI_PASSWORD_CHARACTERISTIC_UUID "605fa5b2-bf86-4e1b-93c0-4c77cf0df75c"
#define WIFI_RECONNECT_CHARACTERISTIC_UUID "92996f42-419a-4162-8b70-96aa7a9d891e"

BLEServer *bleServer;
BLEService *configuratorService;
BLECharacteristic *promptCharacteristic;

std::string currentPrompt = "42.0kts";
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

class PromptCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *characteristic) {
    Serial.println("Reading prompt.");
    characteristic->setValue(currentPrompt);
    delay(1);
  }

  void onWrite(BLECharacteristic *characteristic) {
    Serial.println("Writing prompt.");
    currentPrompt = characteristic->getValue();
    delay(1);
    updateDisplay();
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

void initBLE() {
  BLEDevice::init("m5paper");
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new ConfiguratorServerCallbacks());
  configuratorService = bleServer->createService(SERVICE_UUID);

  promptCharacteristic = configuratorService->createCharacteristic(
    PROMPT_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE
  );
  promptCharacteristic->setCallbacks(new PromptCharacteristicCallbacks());

  createCharacteristicForProperty(
    configuratorService, WIFI_SSID_CHARACTERISTIC_UUID,
    [] () { return WiFiConnection.getSsid(); },
    [] (std::string newSsid) { WiFiConnection.setSsid(newSsid); }
  );
  createCharacteristicForProperty(
    configuratorService, WIFI_PASSWORD_CHARACTERISTIC_UUID,
    [] () { return WiFiConnection.getPass(); },
    [] (std::string newPass) { WiFiConnection.setPass(newPass); }
  );

  auto reconnectCharacteristic = configuratorService->createCharacteristic(
    WIFI_RECONNECT_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  reconnectCharacteristic->setCallbacks(new WiFiReconnectCharacteristicCallback());

  configuratorService->start();
  BLEAdvertising *advertising = bleServer->getAdvertising();
  advertising->start();
}