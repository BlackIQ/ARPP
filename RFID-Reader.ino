/*
      RFID Reader
            By BlackIQ
    Github.com/BlackIQ

  
  Typical pin layout used:
  -----------------------------------------------------------------------------------------
              MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
              Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
  Signal      Pin          Pin           Pin       Pin        Pin              Pin
  -----------------------------------------------------------------------------------------
  RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
  SPI SS      SDA(SS)      10            53        D10        10               10
  SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
  SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
  SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include "SPI.h"
#include "MFRC522.h"

// RFID Pins
#define SS_PIN 10
#define RST_PIN 9

#define green 7
#define red 6

// MFRC522 Setup
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  rfid.PCD_Init();

  // Have some lights
  pinMode(green , OUTPUT);
  pinMode(red , OUTPUT);
}

void loop() {
  // Check the connection
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Getting ID
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }

  // print tag id
  Serial.println(strID);

  // set a Condition
  if (strID.indexOf("Tag ID") >= 0) {
    digitalWrite(green , HIGH);
    Serial.println("Avalilable !");
    delay(5000);
    Serial.println();
    digitalWrite(green , LOW);
  }
  else {
    digitalWrite(red , HIGH);
    Serial.println("Forbidden !");
    delay(5000);
    Serial.println();
    digitalWrite(red , LOW);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
