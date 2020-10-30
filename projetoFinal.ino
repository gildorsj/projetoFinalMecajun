#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9

#define RELE 4
#define LED_AMARELO 5
#define LED_VERDE 6
#define LED_VERMELHO 7

MFRC522 rfidModule(SS_PIN, RST_PIN); 

String registeredTag[10] = {"5bd38a1b","0","0","0","0","0","0","0","694989bb","0"};
String tag = ""; 
int counter = 0;    
bool access = false; 

void setup(){
  Serial.begin(9600);          
  SPI.begin();                  
  rfidModule.PCD_Init();        
  pinMode(LED_VERDE, OUTPUT);   
  pinMode(LED_VERMELHO, OUTPUT); 
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(RELE, OUTPUT);  
}

void loop(){
  if( counter == 3 )
    configurationMode();
  else
    scanMode();

  if( counter == 6 )
    eraseMode();

    
}

void configurationMode(){
  actionConfigurationMode();
  
  if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial()){
    delay(50);
    return;
  }

  tag = "";
  for (byte i = 0; i < rfidModule.uid.size; i++)
    tag.concat(String(rfidModule.uid.uidByte[i], HEX));

  if (tag.equalsIgnoreCase(registeredTag[0])){
    counter++;
    return 0;
  }
  
  for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++){
    if (tag.equalsIgnoreCase(registeredTag[i])){
      counter = 0;
      return;
    }
  }
  
  for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++){
    if (registeredTag[i] == "0"){
      registeredTag[i] = tag;
      actionNewTagRegistered();
      counter = 0;
      Serial.println("New id registered: " + tag);
      return;
    }
  }
}


void eraseTags(){
  for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++){
    registeredTag[i] = "0";
  }
}

void eraseMode(){
  actionEraseMode();
  
  if (!rfidModule.PICC_IsNewCardPresent() || !rfidModule.PICC_ReadCardSerial()){
    delay(50);
    return;
  }

  tag = "";
  for (byte i = 0; i < rfidModule.uid.size; i++)
    tag.concat(String(rfidModule.uid.uidByte[i], HEX));

  if (tag.equalsIgnoreCase(registeredTag[0])){
    Serial.println("Exit erase mode");
    counter = 0;
    return;
  }else{
    for (int i = 1; i < (sizeof(registeredTag) / sizeof(String)); i++){
      if (tag.equalsIgnoreCase(registeredTag[i])){
        eraseTags();
        counter = 0;
        Serial.println("All tags erased");
        return;
      }
    }
  }
}

void scanMode(){
  actionScanMode();
  
  if (registeredTag[0] == "0"){
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
  
  for (int i = 0; i < (sizeof(registeredTag) / sizeof(String)); i++){
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

void accessGranted(){
  Serial.println("Access Granted id: " + tag); 
  actionAccessGranted();                         
  access = false;                          
}

void accessDenid(){
  Serial.println("Access Denid id: " + tag); 
  ActionAccessDenid();                                
}

void actionEraseMode(){
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, HIGH);
  delay(250);
  digitalWrite(LED_VERMELHO, LOW);
  delay(250);
}

void actionScanMode(){
  digitalWrite(LED_AMARELO, HIGH);
}

void actionConfigurationMode(){
  digitalWrite(LED_AMARELO, LOW);
  delay(250);
  digitalWrite(LED_AMARELO, HIGH);
  delay(250);
}

void actionNewTagRegistered(){
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARELO, HIGH);
  digitalWrite(LED_VERMELHO, HIGH);
  delay(300);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  delay(300);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARELO, HIGH);
  digitalWrite(LED_VERMELHO, HIGH);
  delay(300);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  delay(300);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARELO, HIGH);
  digitalWrite(LED_VERMELHO, HIGH);
  delay(300);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  delay(500);
}

void actionAccessGranted(){
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(RELE, HIGH);
  digitalWrite(LED_AMARELO, LOW);
  delay(500);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(RELE, LOW);
  digitalWrite(LED_AMARELO, HIGH);
  delay(500);
}

void ActionAccessDenid(){
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_AMARELO, LOW);
  delay(500);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AMARELO, HIGH);
  delay(500);
}
