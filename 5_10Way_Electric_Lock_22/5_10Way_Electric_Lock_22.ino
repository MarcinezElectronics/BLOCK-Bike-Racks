/* * * * * * * * * * * * * * * *
 * * * Elektronikus kódzár * * *
  * * * * * * 10 dokk  * * * * * 
 * * * * * * * * * * * * * * * *
 * * Készítette: Pál Marcell * *
 * * * * * * * * * * * * * * * *
 * * *  V1 * 2021.08.23. * * * * 
 * * *  V2 * 2021.08.25. * * * * 
 * * *  V3 * 2021.09.01. * * * * 
 * * *  V4 * 2021.09.14. * * * *
 * * *  V5 * 2022.09.03. * * * *
 * * *  V6 * 2022.11.24. * * * *
 * * *  V7 * 2022.11.29. * * * *
 * * *  V8 * 2022.12.08. * * * *
 * * * * * * * * * * * * * * * */

//A 486-os sortól van kikommentelve a 8-9-es dokkválasztó
//A szükséges állomást kell definiálni
//#define TENWAY
#define FIVEWAY

#include <Wire.h>
#include "U8glib.h"
#include <SPI.h>
#include <SD.h>
#include <Keypad.h>

#include <virtuabotixRTC.h>
#include <Time.h>
#include <TimeLib.h>

#define INTERVAL    30000 //5000

//Keypad pinouts
#define R1  4
#define R2  9
#define R3  8
#define R4  6  
#define C1  5
#define C2  3 
#define C3  7

/*//Keypad pinouts (Saját!)
#define R1  6
#define R2  5
#define R3  4
#define R4  3  
#define C1  9
#define C2  8 
#define C3  7*/

//Output pinouts
#define L0  31
#define L1  30
#define L2  29
#define L3  28
#define L4  27
#ifdef TENWAY
#define L5  26
#define L6  25
#define L7  24
#define L8  23
#define L9  22
#endif

//Input pinouts
#define S0  41
#define S1  40
#define S2  39
#define S3  38
#define S4  37
#ifdef TENWAY
#define S5  36
#define S6  35
#define S7  34
#define S8  33
#define S9  32
#endif

#define OK  '#'
#define DEL '*'

#define BUZZER  A14

#define RTC_CLK 11 
#define RTC_DAT 12
#define RTC_RST 13

U8GLIB_KS0108_128 u8g(A0, A1, A2, A3, A4, A5, A6, A7, A13, A9, A8, A12, A11, A10); //U8GLIB(&u8g_dev_ks0108_128x64_fast, d0, d1, d2, d3, d4, d5, d6, d7, en, cs1, cs2, di, rw, reset)
//CS1=1 CS2=0 - left dots
//CS1=0 CS2=1 - right dots
//fel van cserélve a lib-ben! (CS1=A9, CS2=A8)

File myFile;

void fileWrite(String pwFile);
String recoveryPassword(String pw);
void rewritePassword(String rwpw, String newpw);
void watchIdleSwitch();
void watchIdleSensor();
void resetIdleSwitch();
void mainMenu();
void dockSelect();
void dockCheck();
void unlockDock();
void writeUses(unsigned char dockNo, unsigned long ellapsedTime);
boolean scanPassword(unsigned int actuallyDock, String passwordToSave);
void newPassword();
void againPassword();
void logPIN();
void unlockingClosedDock();
void unlockingOpenedDock();
void masterDockSelect();
void masterUnlockDock();
void delPIN();
void drawScreen(char drawState);
void delChar();
void showStar();
void showDock();
void setScreen();
void buzzer();
time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss);
void getRTCtime();
void setRTCtime();

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {R1, R2, R3, R4};
byte colPins[COLS] = {C1, C2, C3};
 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String masterPIN   = "123456";
String setStar     = "";
String setDock     = "";
String setPassword = "";
String secPassword = "";
String password_1  = "";
String password_2  = "";
String password_3  = "";
String password_4  = "";
String password_5  = "";
#ifdef TENWAY
String password_6  = "";
String password_7  = "";
String password_8  = "";
String password_9  = "";
String password_10 = "";
#endif

volatile char key;

boolean i = false;

volatile unsigned char mainBack = 0;

volatile unsigned char adminBack = 0;

volatile unsigned char counter  = 0; 
volatile unsigned char dockInt  = 0;

volatile unsigned long currentMillis  = 0;
volatile unsigned long previousMillis = 0;

volatile unsigned long unixTime   = 0;
volatile unsigned long unixTime1  = 0;
volatile unsigned long unixTime2  = 0;
volatile unsigned long unixTime3  = 0;
volatile unsigned long unixTime4  = 0;
volatile unsigned long unixTime5  = 0;
#ifdef TENWAY
volatile unsigned long unixTime6  = 0;
volatile unsigned long unixTime7  = 0;
volatile unsigned long unixTime8  = 0;
volatile unsigned long unixTime9  = 0;
volatile unsigned long unixTime10 = 0;
#endif

//RTC variables
volatile int    year3    = 0;
volatile int    month3   = 0;
volatile int    day3     = 0;
volatile int    hour3    = 0;
volatile int    min3     = 0;
volatile int    sec3     = 0;
String usingDate = "";

boolean goodSensor = true;

virtuabotixRTC myRTC(RTC_CLK, RTC_DAT, RTC_RST);

void setup(){
  
  setScreen();
   
  pinMode(S0, INPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
#ifdef TENWAY
  pinMode(S5, INPUT);
  pinMode(S6, INPUT);
  pinMode(S7, INPUT);
  pinMode(S8, INPUT);
  pinMode(S9, INPUT);
#endif
  pinMode(L0, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
#ifdef TENWAY
  pinMode(L5, OUTPUT);
  pinMode(L6, OUTPUT);
  pinMode(L7, OUTPUT);
  pinMode(L8, OUTPUT);
  pinMode(L9, OUTPUT);
#endif
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);
  //setRTCtime();

  getRTCtime();
  Serial.print(year3);
  Serial.print(".");
  Serial.print(month3);
  Serial.print(".");
  Serial.print(day3);
  Serial.print(".");
  Serial.print(" ");
  Serial.print(hour3);
  Serial.print(":");
  Serial.print(min3);
  Serial.print(":");
  Serial.print(sec3);
  Serial.println();

//SD kártya inicializálás-----------------------------------------------------------
  drawScreen(0);
  delay(1000);

  while (!SD.begin(53)) { //SD kártya automatikus felismerése
    drawScreen(2);
  }
  
  drawScreen(1);
  delay(1000);

//Fájlok létrehozása, ha nem léteznek------------------------------------------------
  fileWrite("pw1.txt");
  fileWrite("pw2.txt");
  fileWrite("pw3.txt");
  fileWrite("pw4.txt");
  fileWrite("pw5.txt");
#ifdef TENWAY 
  fileWrite("pw6.txt");
  fileWrite("pw7.txt");
  fileWrite("pw8.txt");
  fileWrite("pw9.txt");
  fileWrite("pw10.txt");
#endif
  fileWrite("uses.csv");

//Jelszavak visszanyerése az SD kártyáról pl. áramszünet után------------------------
  password_1  = recoveryPassword("pw1.txt");
  password_2  = recoveryPassword("pw2.txt");
  password_3  = recoveryPassword("pw3.txt");
  password_4  = recoveryPassword("pw4.txt");
  password_5  = recoveryPassword("pw5.txt");
#ifdef TENWAY
  password_6  = recoveryPassword("pw6.txt");
  password_7  = recoveryPassword("pw7.txt");
  password_8  = recoveryPassword("pw8.txt");
  password_9  = recoveryPassword("pw9.txt");
  password_10 = recoveryPassword("pw10.txt");
#endif  
  mainMenu();
}
  
void loop(){   
}

//SD kártya írás, olvasás, felülírás függvényei--------------------------------------
void fileWrite(String pwFile){

  myFile = SD.open(pwFile, FILE_WRITE);

  if (myFile) {
    myFile.close();
  } else {
    drawScreen(3);
    delay(5000);
  }  
}

String recoveryPassword(String pw){

  String seged = "";
  myFile = SD.open(pw);
  if (myFile) {
    while (myFile.available()) {
      char character = myFile.read();
      seged += character;  
    }
    myFile.close();
  } 
  else {
    drawScreen(4);
    delay(5000);
  }
  return seged;
}

void rewritePassword(String rwpw, String newpw){
  
  myFile = SD.open(rwpw, O_READ | O_WRITE | O_CREAT | O_TRUNC);

  if (myFile) {
    myFile.print(newpw);
    myFile.close();
  } else {
    drawScreen(3);
    delay(5000);
  }  
}

//Használatok számának rögzítése az SD kártyára-----------------------------------
void writeUses(unsigned char dockNo, unsigned long ellapsedTime){

  myFile = SD.open("uses.csv", FILE_WRITE);

  if (myFile) {
    myFile.print(dockInt);
    myFile.print(";");
    myFile.print(ellapsedTime);
    myFile.println();
    myFile.close();
  } else {
    drawScreen(3);
    delay(5000);
  }
}

//Nyomógomb tétlenség ellenőrzés függyényei---------------------------------------
void watchIdleSwitch(){

currentMillis = millis();

if((unsigned long)(currentMillis - previousMillis) >= INTERVAL){
  previousMillis = currentMillis;
  i = false;  //Fontos, mert e nélkül visszatérésnél nem lép be a főciklusba! 
  mainMenu();
  }
}

void watchIdleSensor(){

currentMillis = millis();

if((unsigned long)(currentMillis - previousMillis) >= 20000){ //20sec
  previousMillis = currentMillis;
  goodSensor = false; //false esetén rossz a szenzor, így lép ki a while ciklusból
  }
}

void resetIdleSwitch(){
  previousMillis = currentMillis;  
}

//Főmenü---------------------------------------------------------------------------
void mainMenu(){
  
  mainBack    = 0;
  adminBack   = 0;
  counter     = 0;
  dockInt     = 0;
  setDock     = "";
  secPassword = "";
  setPassword = "";
  goodSensor = true;  //feltételezve, hogy jó a szenzor (while ciklus miatt)
  
  dockSelect();
}

//Dokk kiválasztása--------------------------------------------------------------------
void dockSelect(){

i = true;

drawScreen(6);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if (key){
  Serial.println(key); 
  
  resetIdleSwitch(); 
  
  switch(key){
    case '0':
    buzzer();
    if((counter < 2) && (dockInt == 1)){
      setDock += 0;
      counter++;
    }
    else {
      mainBack++;
      if(mainBack > 1){mainMenu();}           //két 0 gombnyomásra vissza a főmenübe
      }
      break;
    
    case '1':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 1;
      counter++;
    }
    else {
      adminBack++;
      if(adminBack > 1){masterDockSelect();}  //két 1-es gombnyomásra vissza az admin felületre
      }
      break;
    
    case '2':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 2;
      counter++;
    }
      break;
    
    case '3':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 3;
      counter++;
    }
      break;
    
    case '4':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 4;
      counter++;
    }
      break;
    
    case '5':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 5;
      counter++;
    }
      break;
#ifdef TENWAY    
    case '6':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 6;
      counter++;
    }
      break;
    
    case '7':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 7;
      counter++;
    }
      break;
    
    case '8':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 8;
      counter++;
    }
      break;
    /*
    case '9':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 9;
      counter++;
    }
      break;*/
#endif    
    case OK:
    buzzer();
    if((counter <= 2) && (counter > 0)){
      counter = 0;
      mainBack = 0;
      adminBack = 0;
      i = false; 
    }
    
    case DEL: //karakter törlés
    buzzer();
    if(counter > 0){
      delChar();
      counter--;
      setDock.remove(counter, 1);
    }
      break;
    
    default:
      break;
  }
  dockInt = setDock.toInt();
  showDock();  
 }
}
  dockCheck();
}

//Induktív érzékelők kezelése---------------------------------------------------------
void dockCheck(){
  
  if      ((digitalRead(S0) == HIGH) && (dockInt == 1)){
    getRTCtime();
    unixTime1 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S0) == LOW)  && (dockInt == 1)){
    getRTCtime();
    unixTime = unixTime - unixTime1;
    unlockDock();
    }
  else if ((digitalRead(S1) == HIGH) && (dockInt == 2)){
    getRTCtime();
    unixTime2 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S1) == LOW)  && (dockInt == 2)){
    getRTCtime();
    unixTime = unixTime - unixTime2;
    unlockDock();
    }
  else if ((digitalRead(S2) == HIGH) && (dockInt == 3)){
    getRTCtime();
    unixTime3 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S2) == LOW)  && (dockInt == 3)){
    getRTCtime();
    unixTime = unixTime - unixTime3;
    unlockDock();
    }
  else if ((digitalRead(S3) == HIGH) && (dockInt == 4)){
    getRTCtime();
    unixTime4 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S3) == LOW)  && (dockInt == 4)){
    getRTCtime();
    unixTime = unixTime - unixTime4;
    unlockDock();
    }
#ifdef FIVEWAY
  else if ((digitalRead(S4) == HIGH) && (dockInt == 5)){
    getRTCtime();
    unixTime5 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S4) == LOW)  && (dockInt == 5)){
    getRTCtime();
    unixTime = unixTime - unixTime5;
    unlockDock();
    }
#endif

#ifdef TENWAY
  else if ((digitalRead(S4) == LOW) && (dockInt == 5)){ //PNP sensor! NPN:HIGH
    getRTCtime();
    unixTime5 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S4) == HIGH)  && (dockInt == 5)){ //PNP sensor! NPN:LOW
    getRTCtime();
    unixTime = unixTime - unixTime5;
    unlockDock();
    }
  else if ((digitalRead(S5) == HIGH) && (dockInt == 6)){
    getRTCtime();
    unixTime6 = unixTime;
    newPassword();}
  else if ((digitalRead(S5) == LOW)  && (dockInt == 6)){
    getRTCtime();
    unixTime = unixTime - unixTime6;
    unlockDock();
    }
  else if ((digitalRead(S6) == HIGH) && (dockInt == 7)){
    getRTCtime();
    unixTime7 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S6) == LOW)  && (dockInt == 7)){
    getRTCtime();
    unixTime = unixTime - unixTime7;
    unlockDock();
    }
  else if ((digitalRead(S7) == LOW) && (dockInt == 8)){ //meg lett cserélve a logika
    getRTCtime();
    unixTime8 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S7) == HIGH)  && (dockInt == 8)){
    getRTCtime();
    unixTime = unixTime - unixTime8;
    unlockDock();
    }
  else if ((digitalRead(S8) == HIGH) && (dockInt == 9)){
    getRTCtime();
    unixTime9 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S8) == LOW)  && (dockInt == 9)){
    getRTCtime();
    unixTime = unixTime - unixTime9;
    unlockDock();
    }
  else if ((digitalRead(S9) == HIGH) && (dockInt == 10)){
    getRTCtime();
    unixTime10 = unixTime;
    newPassword();
    }
  else if ((digitalRead(S9) == LOW)  && (dockInt == 10)){
    getRTCtime();
    unixTime = unixTime - unixTime10;
    unlockDock();
    }
#endif
  else{}   
}

//Zár feloldás------------------------------------------------------------------------
void unlockDock(){

//Ha nincs jelszó-------------------------------------------
      if (scanPassword(dockInt, "") == true){
        drawScreen(15);
        delay(5000);
        unlockingClosedDock(); 
        mainMenu(); //Ha nincs jelszó, és zárva van a kapu (dockCheck alacsony szint), akkor nyitja és visszatér a főmenübe 
      }
//----------------------------------------------------------

setStar = "";

i = true;

drawScreen(7);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if (key){ 

  resetIdleSwitch();

  dockInt = setDock.toInt();
  
  switch(key){
    
    case '0':
    buzzer();
    if(counter < 4){
      secPassword += 0;
      setStar += '*';
      counter++;
    }
    else {
      mainBack++;
      if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe
      }
      break;
    
    case '1':
    buzzer();
    if(counter < 4){
      secPassword += 1;
      setStar += '*';
      counter++;
    }
    else {
      adminBack++;
      if(adminBack > 1){masterDockSelect();}  //két 1-es gombnyomásra vissza az admin felületre
      }
      break;
    
    case '2':
    buzzer();
    if(counter < 4){
      secPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    buzzer();
    if(counter < 4){
      secPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    buzzer();
    if(counter < 4){
      secPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    buzzer();
    if(counter < 4){
      secPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    buzzer();
    if(counter < 4){
      secPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    buzzer();
    if(counter < 4){
      secPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    buzzer();
    buzzer();
    if(counter < 4){
      secPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    buzzer();
    if(counter < 4){
      secPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case OK:
    buzzer();
    if((counter <= 4) && (counter > 0)){

      if (scanPassword(dockInt, secPassword) == true){
        drawScreen(11);
        delay(5000);
        unlockingClosedDock(); 
        counter = 0;
        mainBack = 0;
        adminBack = 0;
        i = false; 
      }
      else if (scanPassword(dockInt, secPassword) == false){
        drawScreen(10);        
        delay(5000);
        counter = 0;
        mainBack = 0;
        adminBack = 0;
        i = true;
        secPassword = "";
        unlockDock();  
      }

      else{} 
    }
      break;
    
    case DEL: //karakter törlés
    buzzer();
    if(counter > 0){
      delChar();     
      counter--;
      secPassword.remove(counter, 1);
      setStar.remove(counter, 1);
    }
      break;
    
    default:
      break;
  }
  showStar();
 }
}
  writeUses(dockInt, unixTime); //Parkolási idő mentése csv fájlba az adott dokkhoz.
  delPIN(); //Jelszó törlése és vissza a főmenübe
}

//Jelszó és dokk ellenőrzés a kiíratáshoz------------------------------------------
boolean scanPassword(unsigned int actuallyDock, String passwordToSave){
  
switch(actuallyDock){
      case 1:
      if (passwordToSave == password_1){return true;}
      else{return false;}
        break; 
      case 2:
      if (passwordToSave == password_2){return true;}
      else{return false;}
        break;       
      case 3:
      if (passwordToSave == password_3){return true;}
      else{return false;}
        break;       
      case 4:
      if (passwordToSave == password_4){return true;}
      else{return false;}
        break;       
      case 5:
      if (passwordToSave == password_5){return true;}
      else{return false;}
        break;
#ifdef TENWAY     
      case 6:
      if (passwordToSave == password_6){return true;}
      else{return false;}
        break;    
      case 7:
      if (passwordToSave == password_7){return true;}
      else{return false;}
        break;       
      case 8:
      if (passwordToSave == password_8){return true;}
      else{return false;}
        break;      
      case 9:
      if (passwordToSave == password_9){return true;}
      else{return false;}
        break;     
      case 10:
      if (passwordToSave == password_10){return true;}
      else{return false;}
       break;
#endif
      default:
        break;
    } 
}

//Új jelszó---------------------------------------------------------------------------
void newPassword(){

setStar = "";

i = true;

drawScreen(8);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if (key){

  resetIdleSwitch();
  
  switch(key){
    
    case '0':
    buzzer();
    if(counter < 4){
      setPassword += 0;
      setStar += '*';
      counter++;
    }
    else {
      mainBack++;
      if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe
      }
      break;
    
    case '1':
    buzzer();
    if(counter < 4){
      setPassword += 1;
      setStar += '*';
      counter++;
    }
    else {
      adminBack++;
      if(adminBack > 1){masterDockSelect();}  //két 1-es gombnyomásra vissza az admin felületre
      }
      break;
    
    case '2':
    buzzer();
    if(counter < 4){
      setPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    buzzer();
    if(counter < 4){
      setPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    buzzer();
    if(counter < 4){
      setPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    buzzer();
    if(counter < 4){
      setPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    buzzer();
    if(counter < 4){
      setPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    buzzer();
    if(counter < 4){
      setPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    buzzer();
    if(counter < 4){
      setPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    buzzer();
    if(counter < 4){
      setPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case OK: //jelszó mentés
    buzzer();
    if((counter <= 4) && (counter > 0)){
      counter = 0;
      mainBack = 0;
      adminBack = 0;
      i = false; 
    }
      break;
    
    case DEL: //karakter törlés
    buzzer();
    if(counter > 0){
      delChar();
      counter--;
      setPassword.remove(counter, 1);
      setStar.remove(counter, 1);
    }
      break;
    
    default:
      break;
    }     
    showStar();    
  }
}
againPassword();
}

//Megerősítő jelszó-------------------------------------------------------------------
void againPassword(){

setStar = "";

i = true;

drawScreen(9);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if (key){ 

  resetIdleSwitch();
  
  switch(key){
    
    case '0':
    buzzer();
    if(counter < 4){
      secPassword += 0;
      setStar += '*';
      counter++;
    }
    else {
      mainBack++;
      if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe
      }
      break;
    
    case '1':
    buzzer();
    if(counter < 4){
      secPassword += 1;
      setStar += '*';
      counter++;
    }
    else {
      adminBack++;
      if(adminBack > 1){masterDockSelect();}  //két 1-es gombnyomásra vissza az admin felületre
      }
      break;
    
    case '2':
    buzzer();
    if(counter < 4){
      secPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    buzzer();
    if(counter < 4){
      secPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    buzzer();
    if(counter < 4){
      secPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    buzzer();
    if(counter < 4){
      secPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    buzzer();
    if(counter < 4){
      secPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    buzzer();
    if(counter < 4){
      secPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    buzzer();
    if(counter < 4){
      secPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    buzzer();
    if(counter < 4){
      secPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case OK: //jelszó mentés
    buzzer();
    if((counter <= 4) && (counter > 0)){
      if(secPassword == setPassword) {
        
        drawScreen(12);
        delay(5000);            //5 másodperc késleltetés a nyitás előtt
        unlockingOpenedDock();  //Nyitás, ha a megadott jelszavak egyeznek (magyar mód)
               
        counter = 0;
        mainBack = 0;
        adminBack = 0;
        i = false;
        }
      else if(secPassword != setPassword){
        
        drawScreen(10);
        delay(3000);
        
        counter = 0;
        mainBack = 0;
        adminBack = 0;
        i = true;
        secPassword = "";
        againPassword();
        }  
    }
      break;
    
    case DEL: //karakter törlés
    buzzer();
    if(counter > 0){
      delChar();
      counter--;
      secPassword.remove(counter, 1);
      setStar.remove(counter, 1);
    }
      break;
    
    default:
      break;
  }
  showStar();
 }
}
  logPIN();    
}  

//Jelszó mentése az SD kártyára------------------------------------------------------
void logPIN(){

switch(dockInt){
    case 1:
      password_1 = secPassword;
      rewritePassword("pw1.txt", password_1);
      break;
    case 2:
      password_2 = secPassword;
      rewritePassword("pw2.txt", password_2);
      break;
    case 3:
      password_3 = secPassword;
      rewritePassword("pw3.txt", password_3);
      break;
    case 4:
      password_4 = secPassword;
      rewritePassword("pw4.txt", password_4);
      break;
    case 5:
      password_5 = secPassword;
      rewritePassword("pw5.txt", password_5);
      break;
#ifdef TENWAY
    case 6:
      password_6 = secPassword;
      rewritePassword("pw6.txt", password_6);
      break;
    case 7:
      password_7 = secPassword;
      rewritePassword("pw7.txt", password_7);
      break;
    case 8:
      password_8 = secPassword;
      rewritePassword("pw8.txt", password_8);
      break;
    case 9:
      password_9 = secPassword;
      rewritePassword("pw9.txt", password_9);
      break;
    case 10:
      password_10 = secPassword;
      rewritePassword("pw10.txt", password_10);
      break;
#endif
  }  
  mainMenu();
}

//Dokk feloldása------------------------------------------------------
void unlockingClosedDock(){

previousMillis = currentMillis; //watchIdleSensor függvénynek

switch(dockInt){
    case 1:
      digitalWrite(L0, HIGH);
      while((!(digitalRead(S0))) && goodSensor){
      watchIdleSensor();
      /*Serial.println(previousMillis);
      Serial.println(currentMillis);
      Serial.println(goodSensor);*/
      }
      delay(3000);
      digitalWrite(L0, LOW);
      break;
    case 2:
      digitalWrite(L1, HIGH);
      while((!(digitalRead(S1))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L1, LOW);
      break;
    case 3:
      digitalWrite(L2, HIGH);
      while((!(digitalRead(S2))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L2, LOW);
      break;
    case 4:
      digitalWrite(L3, HIGH);
      while((!(digitalRead(S3))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L3, LOW);
      break;
    case 5:
      digitalWrite(L4, HIGH);
      while((!(digitalRead(S4))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L4, LOW);
      break;
#ifdef TENWAY
    case 6:
      digitalWrite(L5, HIGH);
      while((!(digitalRead(S5))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L5, LOW);
      break;
    case 7:
      digitalWrite(L6, HIGH);
      while((!(digitalRead(S6))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L6, LOW);
      break;
    case 8:
      digitalWrite(L7, HIGH);
      while((!(digitalRead(S7))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L7, LOW);
      break;
    case 9:
      digitalWrite(L8, HIGH);
      while((!(digitalRead(S8))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L8, LOW);
      break;
    case 10:
      digitalWrite(L9, HIGH);
      while((!(digitalRead(S9))) && goodSensor){
        watchIdleSensor(); 
      }
      delay(3000);
      digitalWrite(L9, LOW);
      break;
#endif
  }
}

//Dokk feloldása a megerősítő jelszó után----------------------------------------
void unlockingOpenedDock(){

switch(dockInt){
    case 1:
      do{
      digitalWrite(L0, HIGH);
      }while(digitalRead(S0));
      digitalWrite(L0, LOW);
      break;
    case 2:
      do{
      digitalWrite(L1, HIGH);
      }while(digitalRead(S1));
      digitalWrite(L1, LOW);
      break;
    case 3:
      do{
      digitalWrite(L2, HIGH);
      }while(digitalRead(S2));
      digitalWrite(L2, LOW);
      break;
    case 4:
      do{
      digitalWrite(L3, HIGH);
      }while(digitalRead(S3));
      digitalWrite(L3, LOW);
      break;
    case 5:
      do{
      digitalWrite(L4, HIGH);
      }while(digitalRead(S4));
      digitalWrite(L4, LOW);
      break;
#ifdef TENWAY
    case 6:
      do{
      digitalWrite(L5, HIGH);
      }while(digitalRead(S5));
      digitalWrite(L5, LOW);
      break;
    case 7:
      do{
      digitalWrite(L6, HIGH);
      }while(digitalRead(S6));
      digitalWrite(L6, LOW);
      break;
    case 8:
      do{
      digitalWrite(L7, HIGH);
      }while(digitalRead(S7));
      digitalWrite(L7, LOW);
      break;
    case 9:
      do{
      digitalWrite(L8, HIGH);
      }while(digitalRead(S8));
      digitalWrite(L8, LOW);
      break;
    case 10:
      do{
      digitalWrite(L9, HIGH);
      }while(digitalRead(S9));
      digitalWrite(L9, LOW);
      break;
#endif
  }
}

//Mester jelszó a feloldáshoz, elfelejtett kód esetén--------------------------------
void masterDockSelect(){

i = true;

drawScreen(13);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if (key){ 
  
  resetIdleSwitch(); 

  dockInt = setDock.toInt();
  
  switch(key){
    
    case '0':
    buzzer();
    if((counter < 2) && (dockInt == 1)){
      setDock += 0;
      counter++;
    }
    else {
      mainBack++;
      if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe
      }
      break;
    
    case '1':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 1;
      counter++;
    }
      break;
    
    case '2':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 2;
      counter++;
    }
      break;
    
    case '3':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 3;
      counter++;
    }
      break;
    
    case '4':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 4;
      counter++;
    }
      break;
    
    case '5':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 5;
      counter++;
    }
      break;
#ifdef TENWAY    
    case '6':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 6;
      counter++;
    }
      break;
    
    case '7':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 7;
      counter++;
    }
      break;
    
    case '8':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 8;
      counter++;
    }
      break;
    
    case '9':
    buzzer();
    if((counter < 2) && (dockInt < 1)){
      setDock += 9;
      counter++;
    }
      break;
#endif    
    case OK:
    buzzer();
    if((counter <= 2) && (counter > 0)){
      counter = 0;
      mainBack = 0;
      i = false; 
    }
    
    case DEL: //karakter törlés
    buzzer();
    if(counter > 0){
      delChar();
      counter--;
      setDock.remove(counter, 1);
    }
      break;
    
    default:
      break;
  }
  showDock();
 }
}
  masterUnlockDock();
}

//-----------------------------------------------------------------------------------
void masterUnlockDock(){

setStar = "";

i = true;

drawScreen(14);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if (key){ 

  resetIdleSwitch();
  
  switch(key){
    
    case '0':
    buzzer();
    if(counter < 6){
      secPassword += 0;
      setStar += '*';
      counter++;
    }
    else {
      mainBack++;
      if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe
      }
      break;
    
    case '1':
    buzzer();
    if(counter < 6){
      secPassword += 1;
      setStar += '*';
      counter++;
    }
      break;
    
    case '2':
    buzzer();
    if(counter < 6){
      secPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    buzzer();
    if(counter < 6){
      secPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    buzzer();
    if(counter < 6){
      secPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    buzzer();
    if(counter < 6){
      secPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    buzzer();
    if(counter < 6){
      secPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    buzzer();
    if(counter < 6){
      secPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    buzzer();
    if(counter < 6){
      secPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    buzzer();
    if(counter < 6){
      secPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case OK:
    buzzer();
    if((counter <= 6) && (counter > 0)){
      if(secPassword == masterPIN) {
        drawScreen(11);
        unlockingClosedDock();
        counter = 0;
        mainBack = 0;
        i = false;
        } 
    }
      break;
    
    case DEL: //karakter törlés
    buzzer();
    if(counter > 0){
      delChar();
      counter--;
      secPassword.remove(counter, 1);
      setStar.remove(counter, 1);
     }
      break;
    
    default:
      break;
  }
  showMasterStar();
 }
}
  delPIN();
}

//Jelszó törlése az SD kártyáról------------------------------------------------------
void delPIN(){

switch(dockInt){
    case 1:
      password_1 = "";
      rewritePassword("pw1.txt", password_1);
      break;
    case 2:
      password_2 = "";
      rewritePassword("pw2.txt", password_2);
      break;
    case 3:
      password_3 = "";
      rewritePassword("pw3.txt", password_3);
      break;
    case 4:
      password_4 = "";
      rewritePassword("pw4.txt", password_4);
      break;
    case 5:
      password_5 = "";
      rewritePassword("pw5.txt", password_5);
      break;
#ifdef TENWAY
    case 6:
      password_6 = "";
      rewritePassword("pw6.txt", password_6);
      break;
    case 7:
      password_7 = "";
      rewritePassword("pw7.txt", password_7);
      break;
    case 8:
      password_8 = "";
      rewritePassword("pw8.txt", password_8);
      break;
    case 9:
      password_9 = "";
      rewritePassword("pw9.txt", password_9);
      break;
    case 10:
      password_10 = "";
      rewritePassword("pw10.txt", password_10);
      break;
#endif
  }
  mainMenu();
}

//Kijelző kezelő függvények---------------------------------------------------------------
void drawScreen(char drawState){ 
  u8g.setFont(u8g_font_courR08); //21 karakter fér ki egy sorba (6-os betűméret)
  u8g.firstPage();
  do {
  switch(drawState){
    case 0:
    u8g.drawStr( 0, 34, "   Inicializ""\xe1""l""\xe1""s...  ");
    break;
    case 1:
    u8g.drawStr( 0, 34, " Inicializ""\xe1""l""\xe1""s k""\xe9""sz! ");
    break;
    case 2:
    u8g.drawStr( 0, 30, " Nem tal""\xe1""lhat""\xf3"" vagy  ");
    u8g.drawStr( 0, 41, " hib""\xe1""s az SD k""\xe1""rtya! ");
    break;
    case 3:
    u8g.drawStr( 0, 30, "    Hiba t""\xf6""rt""\xe9""nt a   ");
    u8g.drawStr( 0, 41, "    f""\xe1""jl ""\xed""r""\xe1""sakor!   ");
    break;
    case 4:
    u8g.drawStr( 0, 30, "    Hiba t""\xf6""rt""\xe9""nt a   ");
    u8g.drawStr( 0, 41, " f""\xe1""jl megnyit""\xe1""sakor! ");
    break;
    case 5:
    u8g.drawStr( 0, 30, " *  Admin fel""\xfc""let *");
    u8g.drawStr( 0, 41, " # Dokk v""\xe1""laszt""\xe1""s #");
    break;
    case 6:
    u8g.drawStr( 0, 26, " \xcd""rd be a feloldani");
    u8g.drawStr( 0, 36, " k""\xed""v""\xe1""nt"" dokk sz""\xe1""m""\xe1""t""!");
    u8g.drawStr( 0, 47, " # OK       * Torl""\xe9""s");  //ö - 94 HEX
    break;
    case 7:
    u8g.drawStr( 0, 25, "   K""\xe9""rlek add meg a   ");
    u8g.drawStr( 0, 36, "    felold""\xe1""shoz a ");
    u8g.drawStr( 0, 47, "     PIN k""\xf3""dodat!       ");
    break;
    case 8:
    u8g.drawStr( 0, 14,  " K""\xe9""rlek adj meg egy 4 ");
    u8g.drawStr( 0, 25, " sz""\xe1""mjegy""\xfc"" PIN k""\xf3""dot! ");
    u8g.drawStr( 0, 36, "A parkol""\xe1""s v""\xe9""gezt""\xe9""vel ");
    u8g.drawStr( 0, 47, "  ezzel fogod tudni   ");
    u8g.drawStr( 0, 58, "  kinyitni a dokkot.  ");
    break;
    case 9:
    u8g.drawStr( 0, 30, " K""\xe9""rlek add meg ism""\xe9""t ");
    u8g.drawStr( 0, 41, "     a PIN k""\xf3""dot!");
    break;
    case 10:
    u8g.drawStr( 0, 21, "   Hopp""\xe1"", ""\xfa""gy t""\xfc""nik   ");
    u8g.drawStr( 0, 32, "rossz k""\xf3""dot adt""\xe1""l meg.");
    u8g.drawStr( 0, 43, " K""\xe9""rlek, pr""\xf3""b""\xe1""ld meg ");
    u8g.drawStr( 0, 54, "        ism""\xe9""t!");
    break;
    case 11:
    u8g.drawStr( 0, 8,  "  Szuper, a bring""\xe1""d ");
    u8g.drawStr( 0, 19, " parkol""\xe1""si folyamata ");
    u8g.drawStr( 0, 30, "     v""\xe9""get ""\xe9""rt!");
    u8g.drawStr( 0, 41, "  K""\xe9""rlek vedd ki a ");
    u8g.drawStr( 0, 52, "  dokkb""\xf3""l ""\xe9""s hagyd");
    u8g.drawStr( 0, 63, "   nyitva a kaput!"); 
    break;
    case 12:
    u8g.drawStr( 0, 14, "   Meg is voln""\xe1""nk!    "); 
    u8g.drawStr( 0, 25, "   K""\xe9""rlek helyezd a   "); 
    u8g.drawStr( 0, 36, "  ker""\xe9""kp""\xe1""rt a(z) ");
    u8g.setPrintPos(100, 36);
    u8g.print(setDock);
    u8g.print('.');
    u8g.drawStr( 0, 47, "  t""\xe1""roloba ""\xe9""s csukd   ");
    u8g.drawStr( 0, 58, "     r""\xe1"" a kaput!      ");
    break;
    case 13:
    u8g.drawStr( 0, 12,  "********Admin********");
    u8g.drawStr( 0, 30, " \xcd""ja be a feloldani");
    u8g.drawStr( 0, 41, " k""\xed""v""\xe1""nt"" dokk sz""\xe1""m""\xe1""t""!");
    break;
    case 14:
    u8g.drawStr( 0, 12, "********Admin********");
    u8g.drawStr( 0, 25, "   K""\xe9""rem adja meg a   ");
    u8g.drawStr( 0, 36, " felold""\xe1""shoz az admin");
    u8g.drawStr( 0, 47, "       jelsz""\xf3""t!       ");
    break;
    case 15:
    u8g.drawStr( 0, 19, "  Nincs folyamatban ");
    u8g.drawStr( 0, 30, "      parkol""\xe1""s! ");
    u8g.drawStr( 0, 41, "    K""\xe9""rlek hagyd ");
    u8g.drawStr( 0, 52, "   nyitva a kaput! ");
    break;
    default:
    break;
  }  
  }while ( u8g.nextPage() );
}

void delChar(){
  do{
    u8g.setFont(u8g_font_osb21);
    u8g.print(" ");}while (u8g.nextPage());  
}

void showStar(){
  u8g.firstPage();
  do{
    u8g.setFont(u8g_font_osb21);
    u8g.setPrintPos(35, 47);
    u8g.print(setStar);
    }while (u8g.nextPage());
}

void showMasterStar(){
  u8g.firstPage();
  do{
    u8g.setFont(u8g_font_osb21);
    u8g.setPrintPos(20, 47);
    u8g.print(setStar);
    }while (u8g.nextPage());
}

void showDock(){
  u8g.firstPage();
  do{
    u8g.setFont(u8g_font_osb21);
    if (dockInt == 10) {u8g.setPrintPos(45, 41);}
    else {u8g.setPrintPos(55, 41);}
    u8g.print(setDock);}while (u8g.nextPage());
}
      
void setScreen(){
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  u8g.setFont(u8g_font_courB08); //21 karakter fér ki egy sorba (6-os betűméret)
}

void buzzer(){
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
}

time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss) { 
  tmElements_t tm;
  tm.Year = YYYY - 1970;
  tm.Month = MM;
  tm.Day = DD;
  tm.Hour = hh;
  tm.Minute = mm;
  tm.Second = ss;
  return makeTime(tm); 
}

void getRTCtime(){

  myRTC.updateTime(); //Ha rossz az idő, akkor lehet, hogy hibás a kvarc vagy alacsony az elem feszültség
  
  day3    = myRTC.dayofmonth;
  month3  = myRTC.month;
  year3   = myRTC.year;
  hour3   = myRTC.hours;
  min3    = myRTC.minutes;
  sec3    = myRTC.seconds;

  unixTime = tmConvert_t(year3, (byte)month3, (byte)day3, (byte)hour3, (byte)min3, (byte)sec3);
}

void setRTCtime(){
  sec3   = 0;
  min3   = 14;
  hour3  = 12;
  day3   = 24;
  month3 = 11;
  year3  = 2022;
  
  myRTC.setDS1302Time(sec3, min3, hour3, 5, day3, month3, year3);  //sec, min, hour, dayofweek(1 sunday, 2 monday...), dayofmonth, month, year

  getRTCtime();
}
