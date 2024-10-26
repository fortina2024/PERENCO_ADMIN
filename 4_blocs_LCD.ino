#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal.h>

// Définition des pins pour le lecteur RFID
#define SS_PIN 10
#define RST_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance du lecteur RFID

// Initialisation de l'écran LCD (adresse 0x27 pour I2C, ajustez si nécessaire)
const int rs = 7, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600); // Communication série
  SPI.begin();        // Initialisation du bus SPI
  mfrc522.PCD_Init(); // Initialisation du lecteur RFID
  lcd.begin();        // Initialisation de l'écran LCD
  lcd.backlight();    // Active le rétro-éclairage de l'écran LCD
  Serial.println("Approchez une carte RFID du lecteur...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Authentifier les blocs (remplacer 'keyA' et 'keyB' par les clés correctes)
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; // Par défaut, la clé est souvent 0xFF pour la plupart des cartes
  }

  // Lecture des blocs spécifiques des secteurs
  byte sector1Block5[16];
  byte sector1Block6[16];
  byte sector2Block8[16];
  byte sector2Block9[16];

  // Lecture du secteur 1, bloc 5
  if (readBlock(1, 5, key, sector1Block5)) {
    Serial.print("Secteur 1, Bloc 5: ");
    displayBlockData(sector1Block5);
  }

  // Lecture du secteur 1, bloc 6
  if (readBlock(1, 6, key, sector1Block6)) {
    Serial.print("Secteur 1, Bloc 6: ");
    displayBlockData(sector1Block6);
  }

  // Lecture du secteur 2, bloc 8
  if (readBlock(2, 8, key, sector2Block8)) {
    Serial.print("Secteur 2, Bloc 8: ");
    displayBlockData(sector2Block8);
  }

  // Lecture du secteur 2, bloc 9
  if (readBlock(2, 9, key, sector2Block9)) {
    Serial.print("Secteur 2, Bloc 9: ");
    displayBlockData(sector2Block9);
  }

  // Affichage sur l'écran LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S1 B5: ");
  lcd.print(byteArrayToHexString(sector1Block5));
  lcd.setCursor(0, 1);
  lcd.print("S2 B9: ");
  lcd.print(byteArrayToHexString(sector2Block9));

  delay(5000); // Attendre avant de lire une autre carte
}

// Fonction pour lire un bloc spécifique
bool readBlock(byte sector, byte block, MFRC522::MIFARE_Key key, byte* buffer) {
  MFRC522::StatusCode status;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_AUTH_KEY_A, sector * 4 + block, &key, &mfrc522.uid);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Erreur d'authentification : ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  status = mfrc522.MIFARE_Read(sector * 4 + block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Erreur de lecture : ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  return true;
}

// Fonction pour afficher les données d'un bloc
void displayBlockData(byte* data) {
  for (byte i = 0; i < 16; i++) {
    Serial.print(data[i] < 0x10 ? " 0" : " ");
    Serial.print(data[i], HEX);
  }
  Serial.println();
}

// Fonction pour convertir un tableau de bytes en chaîne hexadécimale
String byteArrayToHexString(byte* data) {
  String hexString = "";
  for (byte i = 0; i < 16; i++) {
    hexString += String(data[i] < 0x10 ? "0" : "") + String(data[i], HEX);
  }
  return hexString;
}
