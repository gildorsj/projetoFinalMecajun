#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9

#define RELE 4
#define LED_AMARELO 5
#define LED_VERDE 6
#define LED_VERMELHO 7
//Led
int red_light_pin = 4;
int green_light_pin = 3;
int blue_light_pin = 2;

MFRC522 rfidModule(SS_PIN, RST_PIN);

String registeredTag[10] = {"5bd38a1b", "0", "0", "0", "0", "0", "0", "0", "694989bb", "0"};
String tag = "";
int counter = 0;
bool access = false;

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfidModule.PCD_Init();
  pinMode(RELE, OUTPUT);
}

void loop()
{
  if (counter == 3)
    configurationMode();
  else
    scanMode();

  if (counter == 6)
    eraseMode();
}

void configurationMode()
{
  actionConfigurationMode();

  if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
  {
    delay(50);
    return;
  }

  tag = "";
  for (byte i = 0; i < rfidModule.uid.size; i++)
    tag.concat(String(rfidModule.uid.uidByte[i], HEX));

  if (tag.equalsIgnoreCase(registeredTag[0]))
  {
    counter++;
    return 0;
  }

  for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++)
  {
    if (tag.equalsIgnoreCase(registeredTag[i]))
    {
      counter = 0;
      return;
    }
  }

  for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++)
  {
    if (registeredTag[i] == "0")
    {
      registeredTag[i] = tag;
      actionNewTagRegistered();
      counter = 0;
      Serial.println("New id registered: " + tag);
      return;
    }
  }
}

void eraseTags()
{
  for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++)
  {
    registeredTag[i] = "0";
  }
}

void eraseMode()
{
  actionEraseMode();

  if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
  {
    delay(50);
    return;
  }

  tag = "";
  for (byte i = 0; i < rfidModule.uid.size; i++)
    tag.concat(String(rfidModule.uid.uidByte[i], HEX));

  if (tag.equalsIgnoreCase(registeredTag[0]))
  {
    Serial.println("Exit erase mode");
    counter = 0;
    return;
  }
  else
  {
    for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++)
    {
      if (tag.equalsIgnoreCase(registeredTag[i]))
      {
        eraseTags();
        counter = 0;
        Serial.println("All tags erased");
        return;
      }
    }
  }
}

void scanMode()
{
  actionScanMode();

  if (registeredTag[0] == "0")
  {
    configurationMode();
    counter = 3;
  }

  if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
    return;

  tag = "";
  for (byte i = 0; i < rfidModule.uid.size; i++)
    tag.concat(String(rfidModule.uid.uidByte[i], HEX));

  if (tag.equalsIgnoreCase(registeredTag[0]))
    counter++;
  else
    counter = 0;

  for (int i = 0; i < (sizeof(registeredTag) / sizeof(String)); i++)
  {
    if (tag.equalsIgnoreCase(registeredTag[i]))
      access = true;
  }

  Serial.println("Counert: " + counter);

  if (access == true)
    accessGranted();
  else
    accessDenid();

  delay(1000);
}

void accessGranted()
{
  Serial.println("Access Granted id: " + tag);
  actionAccessGranted();
  access = false;
}

void accessDenid()
{
  Serial.println("Access Denid id: " + tag);
  ActionAccessDenid();
}

void actionEraseMode()
{
  RGB_color(0, 0, 0);
  RGB_color(255, 0, 0);
  delay(250);
  RGB_color(0, 0, 0);
  delay(250);
}

void actionScanMode()
{
  RGB_color(255, 0, 255);
}

void actionConfigurationMode()
{
  RGB_color(0, 0, 0);
  delay(250);
  RGB_color(255, 0, 255);
  delay(250);
}

void actionNewTagRegistered()
{
  RGB_color(0, 255, 0);
  delay(300);
  RGB_color(0, 0, 0);
  delay(300);
  RGB_color(0, 255, 0);
  delay(300);
  RGB_color(0, 0, 0);
  delay(300);
  RGB_color(0, 255, 0);
  delay(300);
  RGB_color(0, 0, 0);
  delay(500);
}

void actionAccessGranted()
{
  RGB_color(0, 0, 255);
  digitalWrite(RELE, HIGH);
  RGB_color(0, 0, 0);
  delay(500);
  RGB_color(0, 0, 0);
  digitalWrite(RELE, LOW);
  RGB_color(255, 0, 255);
  delay(500);
}

void ActionAccessDenid()
{
  RGB_color(0, 0, 0);
  RGB_color(0, 0, 0);
  delay(500);
  RGB_color(0, 0, 0);
  RGB_color(255, 0, 255);
  delay(500);
}
