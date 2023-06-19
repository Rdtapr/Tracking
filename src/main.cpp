#include <lorawan.h>
#include <TinyGPSPlus.h>
#include <MFRC522.h>
#include <SPI.h>

String cardIdContainer;

byte sda = 14;
byte rst = 13;

#define TXD2 17
#define RXD2 16

//GPS
TinyGPSPlus gps;

MFRC522 rfid(sda, rst);

//ABP Credential
const char *devAddr = "64842a1d"; // Ganti dengan device adrress masing masing 
const char *appSKey = "64842a1dd9b9c97d3c766d61458474b7"; // Ganti dengan Apps Key masing masing 
const char *nwkSKey = "64842a1dc63216758cb18b2c6b7a48e4"; // Ganti dengan Nwks Key masing masing 

const unsigned long interval = 10000; // interval pengiriman data 10 detik 
unsigned long previousMillis = 0; 

char myStr[50];
char cardStr[50];
byte outStr[255];
int port, channel, freq;
#define MSG_BUFFER_SIZE (150)
// tipe_data#latitude#longitude#plat_nomor
char msg[MSG_BUFFER_SIZE] = "{\"p\":\"%s#%.6f#%.6f#S3637KH\"}";
char card[MSG_BUFFER_SIZE] = "{\"p\":\"%s#%.6f#%.6f#%s#S3637KH\"}";
char pesan[150];
const sRFM_pins RFM_pins = {
 .CS = 5,
 .RST = 0,
 .DIO0 = 27,
 .DIO1 = 2,
};

boolean isGpsValid = false;
void setup() {
 
 Serial.begin(115200);
 Serial2.begin(9600);
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

}
void loop() {
  double lat ;
  double lng ;
  
  while (Serial2.available() > 0){
    gps.encode(Serial2.read());
     if (gps.location.isValid()){
      lat = gps.location.lat();
      lng = gps.location.lng();
      // Serial.print("Latitude= "); 
      // Serial.println(lat);
      // Serial.print(" Longitude= "); 
      // Serial.println(lng);
      if (millis() - previousMillis > interval) {
        isGpsValid = false;
        Serial.println("MENGIRIM DATA KE GATEWAY");
        sprintf(myStr, msg, "pos",lat,lng);
        Serial.print("Sending: ");
        Serial.println(myStr);
        lora.sendUplink(myStr, strlen(myStr), 0);
        port = lora.getFramePortTx();
        channel = lora.getChannel();
        freq = lora.getChannelFreq(channel);
        Serial.print(F("fport: ")); Serial.print(port); Serial.print(" ");
        Serial.print(F("Ch: ")); Serial.print(channel); Serial.print(" ");
        Serial.print(F("Freq: ")); Serial.print(freq); Serial.println(" ");
        previousMillis = millis();
      }
    

    
    }
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        cardIdContainer = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
          cardIdContainer.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : ""));
          cardIdContainer.concat(String(rfid.uid.uidByte[i], HEX));
        }
        sprintf(cardStr, card, "pos",lat,lng, cardIdContainer);
                lora.sendUplink(cardStr, strlen(cardStr), 0);
        port = lora.getFramePortTx();
        channel = lora.getChannel();
        freq = lora.getChannelFreq(channel);
        Serial.print(F("fport: ")); Serial.print(port); Serial.print(" ");
        Serial.print(F("Ch: ")); Serial.print(channel); Serial.print(" ");
        Serial.print(F("Freq: ")); Serial.print(freq); Serial.println(" ");

        delay(2500);
      }
    lora.update();
  }
 
 
 /*
  if (millis() - previousMillis > interval) {
    isGpsValid = false;
    Serial.println("MENGIRIM DATA KE GATEWAY");
    sprintf(myStr, msg, "pos",lat,lng);
    Serial.print("Sending: ");
    Serial.println(myStr);
    lora.sendUplink(myStr, strlen(myStr), 0);
    port = lora.getFramePortTx();
    channel = lora.getChannel();
    freq = lora.getChannelFreq(channel);
    Serial.print(F("fport: ")); Serial.print(port); Serial.print(" ");
    Serial.print(F("Ch: ")); Serial.print(channel); Serial.print(" ");
    Serial.print(F("Freq: ")); Serial.print(freq); Serial.println(" ");
    previousMillis = millis();
  }
 */
 lora.update();


 
}