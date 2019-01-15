#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN   9
#define SS_PIN    10

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Scan card...");
}

void loop() {
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID: ");
  lcd.setCursor(0, 1);
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    uid.concat(String(rfid.uid.uidByte[i], HEX));
    lcd.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    lcd.print(rfid.uid.uidByte[i], HEX);
  }
  char* uidStr = uid.c_str();
  String nuid = "";
  for (int i = 0; i < strlen(uidStr); i++) {
    nuid += (char) toupper(uidStr[i]);
    if (i != 0 && (i-1) % 2 == 0 && i!=(strlen(uidStr)-1)) nuid += " ";
  }
  Serial.write(nuid.c_str());
  Serial.write('\n');
  rfid.PICC_HaltA();
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ) {
    return;
  }
}
