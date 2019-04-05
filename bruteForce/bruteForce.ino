#include <Wire.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal.h>

#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

#define HALT() while(1);

const uint8_t defKeyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
const uint8_t commonKeys[9][6] = {
          {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF},
          {0XD3, 0XF7, 0XD3, 0XF7, 0XD3, 0XF7},
          {0XA0, 0XA1, 0XA2, 0XA3, 0XA4, 0XA5},
          {0XB0, 0XB1, 0XB2, 0XB3, 0XB4, 0XB5},
          {0X4D, 0X3A, 0X99, 0XC3, 0X51, 0XDD},
          {0X1A, 0X98, 0X2C, 0X7E, 0X45, 0X9A},
          {0XAA, 0XBB, 0XCC, 0XDD, 0XEE, 0XFF},
          {0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
          {0XAB, 0XCD, 0XEF, 0X12, 0X34, 0X56},
};

Adafruit_PN532 * nfc;
LiquidCrystal * lcd;


void printKey(uint8_t * key) {
    lcd->setCursor(1, 1);
    for(size_t i = 0; i < 3; i++) {
        lcd->print(" 0x");
        lcd->print(key[i], HEX);
    }
    lcd->setCursor(1, 2);
    for(size_t i = 3; i < 6; i++) {
        lcd->print(" 0x");
        lcd->print(key[i], HEX);
    }
}

void serialKey(uint8_t * key) {
    Serial.print("0x");
    Serial.print(key[0], HEX); Serial.print(" 0x");
    Serial.print(key[1], HEX); Serial.print(" 0x");
    Serial.print(key[2], HEX); Serial.print(" 0x");
    Serial.print(key[3], HEX); Serial.print(" 0x");
    Serial.print(key[4], HEX); Serial.print(" 0x");
    Serial.println(key[5], HEX);
}

int bruteForce(uint8_t * key, size_t i, uint8_t * uid, uint8_t uidLength) {
    if(i ==  0)
        Serial.println("Brute Forcing");
    
    uint8_t success;
    if( i == 5) {
        key[i] = 1;
        do {
            Serial.print("\tTesting key: ");serialKey(key);
            success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
            if(!success)
                key[i]++;
        } while (key[i] < 255 && !success);
        return success;
    } else {
        do {
            success = bruteForce(key, i + 1, uid, uidLength);
            if(!success) {
                key[i]++;
            }
        } while (key[i] < 255 && !success);
        Serial.print(success);
        return success;
    }
}



const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;

void setup() {
    Serial.begin(115200);
    lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
    lcd->begin(20, 4);
    lcd->print("Initializing...");

    pinMode(13, INPUT_PULLUP);
    
    nfc = new Adafruit_PN532(PN532_IRQ, PN532_RESET);
    nfc->begin();
    uint32_t versiondata = nfc->getFirmwareVersion();
    if (! versiondata) {
        lcd->clear();
        lcd->home();
        lcd->print("Didn't find PN53x board");
        HALT();
    } else {
        // Got ok data, print it out!
        lcd->clear();
        lcd->home();
        lcd->print("Found chip PN5"); lcd->print((versiondata >> 24) & 0xFF, HEX);
        lcd->setCursor(0, 1);
        lcd->print("Firmware ver. "); lcd->print((versiondata >> 16) & 0xFF, DEC);
        lcd->print('.'); lcd->print((versiondata >> 8) & 0xFF, DEC);
        lcd->setCursor(0, 2);
    }
    
    // configure board to read RFID tags
    nfc->SAMConfig();
    lcd->setCursor(0,3);
    lcd->print("Waiting for button.");
}

void loop() {
    while(digitalRead(13) == HIGH);
    lcd->clear();
    lcd->home();

    // put your main code here, to run repeatedly:
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success && uidLength == 4) {
        lcd->print("Trying common");
        lcd->setCursor(0,0);
        uint8_t key[6] = {0, 0, 0, 0, 0, 0};
        for (size_t i = 0; i < 9; i++) {
            memcpy(key, commonKeys[i], 6);
            success = nfc->mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, key);
            if(success)
                break;
        }
        success = 0;
        if (success) {
            lcd->print("Found a key:   ");
            printKey(key);
        } else {
            key[0] = 0;
            key[1] = 0;
            key[2] = 0;
            key[3] = 0;
            key[4] = 0;
            key[5] = 0;
            lcd->print("Brute Forcing:");
            success = bruteForce(key, 0, uid, uidLength);
            
            if (success) {
                lcd->home();
                lcd->print("Found a key: ");
                printKey(key);
            } else {
                lcd->home();
                lcd->print("Failed to break");
            }
        }
    } else {
        lcd->print("Error reading tag.");
    }
    lcd->setCursor(0,3);
    lcd->print("Press to retry...");    
}
