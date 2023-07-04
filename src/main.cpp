#include <MFRC522.h>
#include <SPI.h>
#include <TinyGPSPlus.h>
#include <lorawan.h>

#define MSG_BUFFER_SIZE (150)
#define TXD2 17
#define RXD2 16
#define RFID_RST 26
#define RFID_SS 25
#define PANIC_BUTTON 17

// ABP Credential
const char *devAddr = "64842a1d"; // Ganti dengan device adrress masing masing
const char *appSKey =
    "64842a1dd9b9c97d3c766d61458474b7"; // Ganti dengan Apps Key masing masing
const char *nwkSKey =
    "64842a1dc63216758cb18b2c6b7a48e4"; // Ganti dengan Nwks Key masing masing
char myStr[50];
char CARD_STRING[50];
char BUTTON_STRING[50];
// tipe_data#latitude#longitude#plat_nomor
char msg[MSG_BUFFER_SIZE] = "{\"p\":\"%s#%.6f#%.6f#S3637KH\"}";

char CARD_PAYLOAD_STRUCTURE[MSG_BUFFER_SIZE] =
    "{\"p\":\"%s#%.4f#%.3f#%s#S3637KH\"}"; // TIPE (C) ==> CARD
                                           // #LATITUDE#LONGITUDE#CARD
                                           // ID#PLAT NOMOR
char BUTTON_PAYLOAD_STRUCTURE[MSG_BUFFER_SIZE] =
    "{\"p\":\"B#DARURAT#%.4f#%.3f#S3637KH\"}"; // TIPE (B) ==> BUTTON
                                               // STATUS (DARURAT)
                                               // #LATITUDE#LONGITUDE#CARD

// ID#PLAT NOMOR
char pesan[150];
const sRFM_pins RFM_pins = {
    .CS = 5,
    .RST = 0,
    .DIO0 = 27,
    .DIO1 = 2,
};
unsigned long interval = 10000; // interval pengiriman data 10 detik
unsigned long previousMillis = 0;
int port, channel, freq;

String cardIdContainer;
boolean isGpsValid = false;
byte outStr[255];

// CLASS
TinyGPSPlus gps;
MFRC522 rfid(RFID_SS, RFID_RST);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  pinMode(PANIC_BUTTON, INPUT);

  SPI.begin();
  rfid.PCD_Init();

  delay(2000);

  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  lora.setDeviceClass(CLASS_C);
  lora.setDataRate(SF10BW125);
  lora.setFramePortTx(5);
  lora.setChannel(MULTI);
  lora.setTxPower(15);
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);

  Serial.println("WAITING FOR DATA");
}

void loop() {
  // PANIC BUTTON
  if (digitalRead(PANIC_BUTTON) == HIGH) {
    Serial.println("PANIC BUTTON");
    sprintf(BUTTON_STRING, BUTTON_PAYLOAD_STRUCTURE, -6.36310, 106.8220);
    lora.sendUplink(BUTTON_STRING, strlen(BUTTON_STRING), 0);
  }

  // RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    cardIdContainer = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      cardIdContainer.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
      cardIdContainer.concat(String(rfid.uid.uidByte[i], HEX));
    }
    Serial.println(cardIdContainer);
    sprintf(CARD_STRING, CARD_PAYLOAD_STRUCTURE, "C", -6.36310, 106.8220,
            cardIdContainer);
    lora.sendUplink(CARD_STRING, strlen(CARD_STRING), 0);
    delay(2500);
  }

  // GSP HANDLER
  while (Serial2.available()) {
    gps.encode(Serial2.read());
  }

  if (gps.location.isUpdated()) {
    Serial.print("Latitude");
    Serial.println(gps.location.lat());
    Serial.print("Longitude");
    Serial.println(gps.location.lng());

    // SEND DATA TO LORA HERE
  }

  lora.update();
}