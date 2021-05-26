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

// Include Libs
#include <LiquidCrystal_I2C.h> // LCD 16x2 Lib
#include <DS3231.h>            // RTC Lib
#include <Wire.h>              // Bus Lib
#include "DHT.h"               // Temp & Humid Lib
#include <SPI.h>               // RFID Lib
#include <MFRC522.h>           // RFID Lib

// DHT Pins
#define DHTPIN A0
#define DHTTYPE DHT11

// RFID Pins
#define SS_PIN 10
#define RST_PIN 9

// Speaker Pins
#define alarm 8

// Lights Pins
#define white 7
#define red 6
#define blue 5
#define green 4

// Relay Pins
#define relay1 3
#define relay2 2

// Sensors Setup
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DHT dht(DHTPIN, DHTTYPE);          // DHT
DS3231 rtc(SDA , SCL);             // RTC
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  // Begin Sensors & Modules
  Serial.begin(9600);  // Serial
  dht.begin();         // DHT
  rtc.begin();         // RTC
  lcd.init();          // LCD
  SPI.begin();         // RFID

  // Light the backlight
  lcd.backlight();

  // Start RFID
  mfrc522.PCD_Init();

  // Setup Digital Pins
  pinMode(green, OUTPUT);   // Green for available RFID Tag
  pinMode(red, OUTPUT);     // Red for Forbidden RFID Tag
  pinMode(blue, OUTPUT);    // Blue for night light
  pinMode(white, OUTPUT);   // White for test
  pinMode(alarm , OUTPUT);  // Alarm Pin ( Speaker )
  pinMode(relay1 , OUTPUT); // Relay 1
  pinMode(relay2 , OUTPUT); // Relay 2

  // RTC Setup
  rtc.setDOW(TUESDAY);        // Set Day-of-Week
  rtc.setTime(21, 28, 0);     // Set the time( 24 hr format )
  rtc.setDate(25, 5, 2021);   // Set the date to ( Month, Day, Year )

  // Off the Relay
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
}

bool lamp = false;
bool night = true;
 
void loop() {
  if (lamp == false) {
    digitalWrite(relay1, HIGH);
  }
  else {
    digitalWrite(relay1, LOW);
  }
  
  lcd.setCursor(0 , 0);
  lcd.print("What the fuck ?");
  lcd.setCursor(0, 1);
  
  if (lamp == true) {
    lcd.print("Light is on :)");
  }
  else {
    lcd.print("Light is off :(");
  }
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  String content= "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  content.toUpperCase();
  
  if (content.substring(1) == "63 19 BA 3E") {
    if (lamp == true) {
      lamp = false;
      tone(alarm, 500, 500);
      digitalWrite(blue, HIGH);
      delay(1000);
      digitalWrite(blue, LOW);
      lcd.clear();
    }
    else {
      lamp = true;
      tone(alarm, 500, 500);
      digitalWrite(green, HIGH);
      delay(1000);
      digitalWrite(green, LOW);
      lcd.clear();
    }
  }
  else {
    tone(alarm, 500, 500);
    digitalWrite(red, HIGH);
    delay(1000);
    digitalWrite(red, LOW);
    tone(alarm, 500, 500);
    
    if (night == true) {
      digitalWrite(white, HIGH);
      night = false;
    }
    else {
      digitalWrite(white, LOW);
      night = true;
    }
  }
  
  delay(1000);
}
