#include "NFC.h"

NFC::NFC (uint8_t IrqPin, uint8_t ResetPin){
  nfc = new Adafruit_PN532(IrqPin, ResetPin);
  nfc->begin();
}

NFC::~NFC() {
  delete(nfc);
}

bool NFC::isConnected() {
  return nfc->getFirmwareVersion();
}

uint32_t NFC::getFirmwareVersion() {
  return nfc->getFirmwareVersion();
}
