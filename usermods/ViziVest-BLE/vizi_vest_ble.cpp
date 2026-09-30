#include "wled.h"
#include "NimBLEDevice.h"

#define VIZIVEST_SERVICE_UUID        "7a9e0001-1234-4abc-8def-123456789abc"
#define VIZIVEST_CHARACTERISTIC_UUID "7a9e0002-1234-4abc-8def-123456789abc"

class ViziVestBLE : public Usermod {

private:
  NimBLECharacteristic* commandCharacteristic = nullptr;

  volatile uint8_t pendingCommand = 0;

  // ViziVest WLED preset numbers
  // Change these later if your actual preset numbers are different.
  const uint8_t PRESET_GLOW   = 1;
  const uint8_t PRESET_LEFT   = 2;
  const uint8_t PRESET_RIGHT  = 3;
  const uint8_t PRESET_HAZARD = 4;
  const uint8_t PRESET_OFF    = 5;

  class CommandCallbacks : public NimBLECharacteristicCallbacks {
  public:
    ViziVestBLE* parent;

    CommandCallbacks(ViziVestBLE* p) {
      parent = p;
    }

    void onWrite(NimBLECharacteristic* characteristic,
                 NimBLEConnInfo& connInfo) override {

      std::string value = characteristic->getValue();

      if (value == "GLOW") {
        parent->pendingCommand = 1;
      }
      else if (value == "LEFT") {
        parent->pendingCommand = 2;
      }
      else if (value == "RIGHT") {
        parent->pendingCommand = 3;
      }
      else if (value == "HAZARD") {
        parent->pendingCommand = 4;
      }
      else if (value == "OFF") {
        parent->pendingCommand = 5;
      }
    }
  };

  CommandCallbacks* callbacks = nullptr;

public:

  void setup() override {

    NimBLEDevice::init("VIZIVEST");

    NimBLEServer* server = NimBLEDevice::createServer();

    NimBLEService* service =
      server->createService(VIZIVEST_SERVICE_UUID);

    commandCharacteristic =
      service->createCharacteristic(
        VIZIVEST_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE
      );

    callbacks = new CommandCallbacks(this);
    commandCharacteristic->setCallbacks(callbacks);

    commandCharacteristic->setValue("VIZIVEST");

    service->start();

    NimBLEAdvertising* advertising =
      NimBLEDevice::getAdvertising();

    advertising->addServiceUUID(VIZIVEST_SERVICE_UUID);
    advertising->start();
  }

  void loop() override {

    uint8_t command = pendingCommand;

    if (command == 0) return;

    pendingCommand = 0;

    switch (command) {

      case 1:
        applyPreset(PRESET_GLOW, CALL_MODE_DIRECT_CHANGE);
        break;

      case 2:
        applyPreset(PRESET_LEFT, CALL_MODE_DIRECT_CHANGE);
        break;

      case 3:
        applyPreset(PRESET_RIGHT, CALL_MODE_DIRECT_CHANGE);
        break;

      case 4:
        applyPreset(PRESET_HAZARD, CALL_MODE_DIRECT_CHANGE);
        break;

      case 5:
        applyPreset(PRESET_OFF, CALL_MODE_DIRECT_CHANGE);
        break;
    }
  }

  uint16_t getId() override {
    return 0x5642;
  }
};

ViziVestBLE viziVestBLE;

REGISTER_USERMOD(viziVestBLE);
