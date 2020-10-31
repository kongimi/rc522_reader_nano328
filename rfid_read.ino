/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <avr/wdt.h>

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 
MFRC522::StatusCode status;

int LED_READ = 6;
int LED_POWER = 3;
int SuperBuzzer = 5;
int test = 0;
int count=0;
int ledValue;
int max_count=6;
int event=0;

void(* resetFunc) (void) = 0;

ISR(WDT_vect){
  resetFunc();
}

void setup() { 
  
  cli();
  MCUSR = 0;
  WDTCSR |= 0b00011000;
  WDTCSR = 0b01000000 | WDTO_2S;
  sei();
  wdt_reset();
  Serial.begin(19200);

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  pinMode(LED_READ, OUTPUT);
  pinMode(SuperBuzzer, OUTPUT);
  pinMode(LED_POWER, OUTPUT);

  LED_ON_OFF(1);
  digitalWrite(LED_POWER,HIGH);
  Buzzer_ON_OFF(1);
  delay(150);
  Buzzer_ON_OFF(0);
}
 
void loop() {
  wdt_reset();
  
  if(event==0){
    analogWrite (LED_POWER , 255);
    event=1;
  }else{
    analogWrite (LED_POWER , 0);
    event=0;
  }
  delay(500);
  // Look for new cards
  test = rfid.PICC_IsNewCardPresent();
  if (test){
    // Verify if the NUID has been readed
    test = rfid.PICC_ReadCardSerial();
    if (test){
      LED_ON_OFF(0);
      Buzzer_ON_OFF(1);
      printHex(rfid.uid.uidByte, rfid.uid.size);
      delay(300);
      LED_ON_OFF(1);
      Buzzer_ON_OFF(0);
      // Halt PICC
      rfid.PICC_HaltA();
      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();
    }
  }
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}

void LED_ON_OFF(int u){
  if(u==0){
    digitalWrite(LED_READ,HIGH);
  }else{
    digitalWrite(LED_READ,LOW);
  }
}

void Buzzer_ON_OFF(int u){
  if(u==1){
    digitalWrite(SuperBuzzer,HIGH);
  }else{
    digitalWrite(SuperBuzzer,LOW);
  }
}
