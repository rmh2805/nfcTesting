#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

class NFC {
  public:
    NFC (uint8_t IrqPin, uint8_t ResetPin);
    ~NFC();

    bool isConnected();
    uint32_t getFirmwareVersion();
  private:
    Adafruit_PN532* nfc;
};
