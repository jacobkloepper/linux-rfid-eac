/**************************************************************************/
/*! 
  Using Arduino Mega uC, Adafruit PN532 RFID reader, and Adafruit TCA9548A I2C MUX
  WIP: can multiplex to one of the readers but the second does not work.
    

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

//defs
#define TCAADDR 0x70

// Digital pins 
// NB. this defines the pin on the uC.
// the PN532 SHIELD is set up to always output IRQ on digital 2.
//   therefore, connecting both shields' digital 2 pins to arduino d2 and d4 respectively,
//   we get desired behaviour.
#define I2C2_IRQ (2)
#define I2C4_IRQ (4)
#define PN532_RESET (3) 

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc_I2C2(I2C2_IRQ, PN532_RESET);
Adafruit_PN532 nfc_I2C4(I2C4_IRQ, PN532_RESET);

// I2C MUX helper
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}


void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero

  Serial.println("Hello!");

  tcaselect(2);
  nfc_I2C2.begin();
  
  tcaselect(4);
  nfc_I2C4.begin();

  tcaselect(2);
  uint32_t versiondata = nfc_I2C2.getFirmwareVersion();
  
  tcaselect(4);
  uint32_t versiondata2 = nfc_I2C4.getFirmwareVersion();
  
  if (!versiondata || !versiondata2) {
    Serial.print("Didn't find one of the PN53x boards");
    while (1); // halt
  }
  
  // configure board to read RFID tags
  tcaselect(2);
  nfc_I2C2.SAMConfig();
  
  tcaselect(4);
  nfc_I2C4.SAMConfig();

  // Set up both readers to read passively
  uint8_t success_I2C2;
  uint8_t success_I2C4;
  
  tcaselect(2);
  success_I2C2 = nfc_I2C2.startPassiveTargetIDDetection(115200);
  if (!success_I2C2) {
    Serial.println("I2C2 not reading.");
  }
  tcaselect(4);
  success_I2C4 = nfc_I2C4.startPassiveTargetIDDetection(115200);
  if (!success_I2C4) {
    Serial.println("I2C4 not reading.");
  }
  
  Serial.println("Ready ...");
}


void loop(void) {
  uint8_t uid_I2C2[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uid_I2C4[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)


  // print out if read
  if (nfc_I2C2.readDetectedPassiveTargetID(uid_I2C2, &uidLength)) {
    tcaselect(2);
 
    Serial.print(" I2C2: ");
    nfc_I2C2.PrintHex(uid_I2C2, uidLength);
    Serial.println("");
  }

  if (nfc_I2C4.readDetectedPassiveTargetID(uid_I2C4, &uidLength)) {
    tcaselect(4);
 
    Serial.print(" I2C4: ");
    nfc_I2C4.PrintHex(uid_I2C4, uidLength);
    Serial.println("");
  }
}