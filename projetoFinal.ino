#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9

#define RELE 4
#define BLUE_LIGHT 5
#define GREEN_LIGHT 6
#define RED_LIGHT 7

#define EEPROMSpace 10

MFRC522 rfidModule(SS_PIN, RST_PIN);

String tag = "";
int counter = 0;
bool access = false;

String getTag(int index);
void putTag(int index, String tag);
void configurationMode();
void eraseTags();
void eraseAll();
void eraseMode();
void eraseAllMode();
void scanMode();
void accessGranted();
void accessDenid();
void actionEraseMode();
void actionErasAllMode();
void actionScanMode();
void actionConfigurationMode();
void actionConfirmed();
void actionAccessGranted();
void ActionAccessDenid();
void RGB_color(int red_light_value, int green_light_value, int blue_light_value);

void setup()
{

   //Limpeza da EEPROM (Executado apenas para limpar a memoria)
   for (int i = 0; i < EEPROMSpace * 8 ; i++) {
       EEPROM.write(i, 0);
   }

   Serial.begin(9600);
   SPI.begin();
   rfidModule.PCD_Init();
   pinMode(BLUE_LIGHT, OUTPUT);
   pinMode(GREEN_LIGHT, OUTPUT);
   pinMode(RED_LIGHT, OUTPUT);
   pinMode(RELE, OUTPUT);
}

void loop()
{
   switch (counter)
   {
   case 3:
      configurationMode();
      break;
   case 6:
      eraseMode();
      break;
   case 10:
      eraseAllMode();
      break;
   default:
      scanMode();
   }
}

String getTag(int index)
{
   String tag = "";
   for (int i = index * 8; i < index * 8 + 8; i++)
      tag.concat(char(EEPROM.read(i)));
   return tag;
}

void putTag(int index, String tag)
{
   for (int i = index, j = 0; i < index + 8; i++, j++)
      EEPROM.write(i, tag[j]);
}

void configurationMode()
{
   actionConfigurationMode();
   delay(1000);

   if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
   {
      delay(50);
      return;
   }

   tag = "";
   for (byte i = 0; i < rfidModule.uid.size; i++)
      tag.concat(String(rfidModule.uid.uidByte[i], HEX));

   if (tag.equalsIgnoreCase(getTag(0)))
   {
      counter++;
      return 0;
   }

   for (int i = 1; i < EEPROMSpace; i++)
   {
      if (tag.equalsIgnoreCase(getTag(i)))
      {
         counter = 0;
         return;
      }
   }

   for (int i = 0; i < EEPROM.length(); i = i + 8)
   {
      if (EEPROM.read(i) == 0)
      {
         putTag(i, tag);
         actionConfirmed();
         counter = 0;
         Serial.println("New id registered: " + tag);
         return;
      }
   }
}

void eraseTags()
{
   for (int i = 8; i < EEPROMSpace * 8; i++)
   {
      EEPROM.write(i, 0);
   }
}

void eraseAll()
{
   for (int i = 0; i < EEPROMSpace * 8; i++)
   {
      EEPROM.write(i, 0);
   }
}

void eraseMode()
{
   delay(1000);
   actionEraseMode();

   if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
   {
      delay(50);
      return;
   }

   tag = "";
   for (byte i = 0; i < rfidModule.uid.size; i++)
      tag.concat(String(rfidModule.uid.uidByte[i], HEX));

   if (tag.equalsIgnoreCase(getTag(0)))
   {
      counter++;
      return 0;
      Serial.println("Exit erase tag mode");
   }

   for (int i = 1; i < EEPROMSpace; i++)
   {
      if (tag.equalsIgnoreCase(getTag(i)))
      {
         eraseTags();
         actionConfirmed();
         Serial.println("Tags erased.");
         counter = 0;
         return;
      }
   }
}

void eraseAllMode()
{
   delay(1000);
   actionErasAllMode();

   if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
   {
      delay(50);
      return;
   }

   tag = "";
   for (byte i = 0; i < rfidModule.uid.size; i++)
      tag.concat(String(rfidModule.uid.uidByte[i], HEX));

   if (tag.equalsIgnoreCase(getTag(0)))
   {
      Serial.println("Exit erase all mode");
      counter = 0;
      return;
   }
   else
   {
      for (int i = 0; i < EEPROMSpace; i++)
      {
         if (tag.equalsIgnoreCase(getTag(i)))
         {
            eraseAll();
            actionConfirmed();
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

   if (EEPROM.read(0) == 0)
   {
      counter = 3;
   }

   if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial())
      return;

   tag = "";
   for (byte i = 0; i < rfidModule.uid.size; i++)
      tag.concat(String(rfidModule.uid.uidByte[i], HEX));

   if (tag.equalsIgnoreCase(getTag(0)))
      counter++;
   else
      counter = 0;

   for (int i = 0; i < EEPROMSpace; i++)
   {
      if (tag.equalsIgnoreCase(getTag(i)))
         access = true;
   }

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

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
   analogWrite(RED_LIGHT, red_light_value);
   analogWrite(GREEN_LIGHT, green_light_value);
   analogWrite(BLUE_LIGHT, blue_light_value);
}

void actionEraseMode()
{
   RGB_color(255, 255, 0);
   delay(400);
   RGB_color(0, 0, 0);
}
void actionErasAllMode()
{
   RGB_color(255, 0, 0);
   delay(400);
   RGB_color(0, 0, 0);
}
void actionScanMode()
{
   RGB_color(255, 255, 0);
}

void actionConfigurationMode()
{
   RGB_color(0, 255, 0);
   delay(400);
   RGB_color(0, 0, 0);
}

void actionConfirmed()
{
   RGB_color(0, 0, 255);
   delay(300);
   RGB_color(0, 0, 0);
   delay(300);
   RGB_color(0, 0, 255);
   delay(300);
   RGB_color(0, 0, 0);
   delay(300);
   RGB_color(0, 0, 255);
   delay(300);
   RGB_color(0, 0, 0);
   delay(300);
}

void actionAccessGranted()
{
   RGB_color(0, 255, 0);
   digitalWrite(RELE, HIGH);
   delay(500);
   digitalWrite(RELE, LOW);
}

void ActionAccessDenid()
{
   RGB_color(255, 0, 0);
   delay(500);
}
