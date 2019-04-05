#ifndef  _CONSTANTS_H_
#define  _CONSTANTS_H_

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
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

#endif //_CONSTANTS_H_
