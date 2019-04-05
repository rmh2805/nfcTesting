#include <Wire.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal.h>
#include "constants.h"
#include "utils.h"


// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 * nfc;
LiquidCrystal * lcd;

const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;

void setup() {
    lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
    lcd->begin(20, 4);
    lcd->print("Initializing...");

    pinMode(13, INPUT_PULLUP);
    
    nfc = new Adafruit_PN532(PN532_IRQ, PN532_RESET);
    nfc->begin();
    uint32_t versiondata = nfc->getFirmwareVersion();
    if (! versiondata) {
        lcdReset(lcd);
        lcd->print("Didn't find PN53x board");
        HALT();
    } else {
        // Got ok data, print it out!
        lcdReset(lcd);
        lcd->print("Found chip PN5"); lcd->print((versiondata >> 24) & 0xFF, HEX);
        lcd->setCursor(0, 1);
        lcd->print("Firmware ver. "); lcd->print((versiondata >> 16) & 0xFF, DEC);
        lcd->print('.'); lcd->print((versiondata >> 8) & 0xFF, DEC);
        lcd->setCursor(0, 2);
    }
    
    // configure board to read RFID tags
    nfc->SAMConfig();
    lcd->setCursor(0,3);
    lcd->print("Waiting for tag");
}

void loop() {
    while(digitalRead(13) == HIGH);
    
    // put your main code here, to run repeatedly:
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
        lcdReset(lcd);

        if(uidLength == 4) {
            lcd->print("UID Dec:");
            lcd->setCursor(2, 1);
            lcd->print(uid[0], DEC);
            for (size_t i = 1; i < uidLength; i++) {
                lcd->print(" ");
                lcd->print(uid[i], DEC);
            }
            lcd->setCursor(0,2);
        
            success = nfc->mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, defKeyA);
            if (success) {
                uint8_t data[16];
                success = nfc->mifareclassic_ReadDataBlock(4, data);
                if (success) {
                    lcd->print("Block 4: ");
                    lcd->setCursor(2,3);
                    for(size_t i = 0; i < 16; i++) {
                        char next = char(data[i]);
                        if(next)
                            lcd->print(next);
                    }
                } else {
                    lcd->print("Failed to read block");
                    lcd->setCursor(0,3);
                    lcd->print(4, DEC);
                }
            } else {
                lcd->print("Unable to auth.");
                lcd->setCursor(0,3);
                size_t i = 0;
                for(i = 0; i < 9; i++) {
                    success = nfc->mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, commonKeys[i]);
                    if(success) {
                        lcd->print("Key "); lcd->print(i); lcd->print(" worked");
                        break;
                    }
                }
                if(i >= 9) {
                    lcd->print("No common keys work.");
                }
            }
        } else {
            lcd->print("Unknown tag format.");
        }
        /*
        if (uidLength == 4) {
            // We probably have a Mifare Classic card ...
            Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

            // Now we need to try to authenticate it for read/write access
            // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
            Serial.println("Trying to authenticate block 4 with default KEYA value");
            uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
            
            // Start with block 4 (the first block of sector 1) since sector 0
            // contains the manufacturer data and it's probably better just
            // to leave it alone unless you know what you're doing
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

            if (success) {
                Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
                uint8_t data[16];
                
                // If you want to write something to block 4 to test with, uncomment
                // the following line and this text should be read back in a minute
                //memcpy(data, (const uint8_t[]) {'R', 'a', 'y', 'm', 'o', 'n', 'd', ' ', 'H', 'e', 'a', 'l', 'y', 0, 0, 0}, sizeof data);
                //success = nfc.mifareclassic_WriteDataBlock (4, data);
                
                // Try to read the contents of block 4
                success = nfc.mifareclassic_ReadDataBlock(4, data);
                
                if (success) {
                    // Data seems to have been read ... spit it out
                    Serial.println("Reading Block 4:");
                    nfc.PrintHexChar(data, 16);
                    Serial.println("");
                    
                    // Wait a bit before reading the card again
                    delay(1000);
                } else {
                    Serial.println("Ooops ... unable to read the requested block.  Try another key?");
                }
            } else {
                Serial.println("Ooops ... authentication failed: Try another key?");
            }
        }
        */
        delay(1000);
    }
}
