/**************************************************************************/
/*   Based on ReadMifare.ino by Adafruit
     Downscaled to fit the needs of a simple RFID in/out logger

     TODO:
      - Implement visual debugging (Blinking LEDs probably)
*/
/**************************************************************************/
// Imports
#include <Wire.h>
#include <Adafruit_PN532.h> // NB: NEED TO DOWNLOAD FROM ADAFRUIT GITHUB

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)

// Adafruit reader setup on the digital pins given
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // configure and start reading RFID tags
  nfc.begin();
  nfc.SAMConfig();
}

// DATA PAYLOAD:
//
// [..7 byte uid..][..1 byte state..] = [..8 byte payload..]
//

void loop(void) {
  uint8_t success;                            // acts as bool: if card read.
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };    // Buffer to store the returned UID
  uint8_t uidLength;                          // buffer for card length: set to 4 or 7 in readPassiveTargetID()
  bool DIRECTION = false;                     // in/out flag: true = entering, false = exiting.
                                              // Set for each reader. TODO: with multiplexed i2c readers, can simply reset variable in each reader func.

  // Read card
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  // Send card bits if read
  if (success) {
    // Print bits
    // Force uidLength to 4 bytes
    //  Because uid[] is zero-initialized, any 4-byte UIDs are post-padded with zeroes (this is desired behaviour)
    uidLength = 7;

    for (int i = 0; i < uidLength; i++) {
      // Write one byte of uid
      // Because uid is zero-initialized, leading zeroes are in by default.
      Serial.write(uid + (i * sizeof(uid[i])), 1);
    }

    // finally, write the state byte
    // 0xFF = entering
    // 0x88 = leaving
    if (DIRECTION == true) {
      Serial.write(0xFF);
    } else {
      Serial.write(0x88);
    }

    // Get ready for next loop
    //Serial.print('\n');
  }

  delay(500);
  Serial.flush();
}