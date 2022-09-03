/* * * * * * * * * * * * * * * *
 * * * Elektronikus kódzár * * *
 * * * * * * * * * * * * * * * *
 * * Készítette: Pál Marcell * *
 * * * * * * * * * * * * * * * *
 * * *  V1 * 2021.08.23. * * * * 
 * * *  V2 * 2021.08.25. * * * * 
 * * *  V3 * 2021.09.01. * * * * 
 * * *  V4 * 2021.09.14. * * * *
 * * * * * * * * * * * * * * * */

#include <Wire.h>
#include "U8glib.h"
#include <SPI.h>
#include <SD.h>
#include <Keypad.h>

#define INTERVAL    30000 //5000

U8GLIB_KS0108_128 u8g(22, 24, 26, 28, 30, 32, 34, 36,  48, 40, 38, 46, 44); //

File myFile;

void fileWrite(String pwFile);
String recoveryPassword(String pw);
void rewritePassword(String rwpw, String newpw);
void watchIdleSwitch();
void resetIdleSwitch();
void mainMenu();
void dockSelect();
void dockCheck();
void unlockDock();
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

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {6, 5, 4, 3};
byte colPins[COLS] = {9, 8, 7};
 
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
String password_6  = "";
String password_7  = "";
String password_8  = "";
String password_9  = "";
String password_10 = "";

volatile char key;

boolean i = false;

volatile unsigned char mainBack = 0;
volatile unsigned char language = 0;

volatile unsigned int  counter  = 0; 
volatile unsigned int  dockInt  = 0;

volatile unsigned long currentMillis  = 0;
volatile unsigned long previousMillis = 0;

void setup(){
  
  setScreen();
   
  pinMode(23, INPUT);
  pinMode(25, INPUT);
  pinMode(27, INPUT);
  pinMode(29, INPUT);
  pinMode(31, INPUT);
  pinMode(33, INPUT);
  pinMode(35, INPUT);
  pinMode(37, INPUT);
  pinMode(39, INPUT);
  pinMode(41, INPUT);

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A8, OUTPUT);
  pinMode(A9, OUTPUT);

//SD kártya inicializálás-----------------------------------------------------------
  drawScreen(0);
  delay(1000);

  if (!SD.begin(53)) {
    drawScreen(2);
    while (1);
  }
  
  drawScreen(1);
  delay(1000);

//Fájlok létrehozása, ha nem léteznek------------------------------------------------
  fileWrite("pw1.txt");
  fileWrite("pw2.txt");
  fileWrite("pw3.txt");
  fileWrite("pw4.txt");
  fileWrite("pw5.txt");
  fileWrite("pw6.txt");
  fileWrite("pw7.txt");
  fileWrite("pw8.txt");
  fileWrite("pw9.txt");
  fileWrite("pw10.txt");

//Jelszavak visszanyerése az SD kártyáról pl. áramszünet után------------------------
  password_1  = recoveryPassword("pw1.txt");
  password_2  = recoveryPassword("pw2.txt");
  password_3  = recoveryPassword("pw3.txt");
  password_4  = recoveryPassword("pw4.txt");
  password_5  = recoveryPassword("pw5.txt");
  password_6  = recoveryPassword("pw6.txt");
  password_7  = recoveryPassword("pw7.txt");
  password_8  = recoveryPassword("pw8.txt");
  password_9  = recoveryPassword("pw9.txt");
  password_10 = recoveryPassword("pw10.txt");
  
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

//Nyomógomb tétlenség ellenőrzés függyényei---------------------------------------
void watchIdleSwitch(){

currentMillis = millis();

if((unsigned long)(currentMillis - previousMillis) >= INTERVAL){
  previousMillis = currentMillis;
  mainMenu();
  }
}

void resetIdleSwitch(){
  previousMillis = currentMillis;  
}

//Főmenü---------------------------------------------------------------------------
void mainMenu(){

  mainBack = 0;
  
  language = 0;

  drawScreen(5);
  
  while(language == 0){
    key = keypad.getKey();
    switch(key){
      case '0':
        counter  = 0;
        dockInt  = 0;
        setDock  = "";
        secPassword = "";
        masterDockSelect();
        break;     
      case '*':
            language = 1;
            counter  = 0;
            dockInt  = 0;
            setDock  = ""; 
            setPassword = "";
            secPassword = "";
            dockSelect();
        break;
      case '#':
            language = 2;
            counter  = 0;
            dockInt  = 0;
            setDock  = "";
            setPassword = "";
            secPassword = ""; 
            dockSelect();
        break;
      default:
            language = 0;
        break;
    }  
  } 
}

//Dokk kiválasztása--------------------------------------------------------------------
void dockSelect(){

i = true;

if      (language == 1) {drawScreen(6);}
else if (language == 2) {drawScreen(15);}

while(i){

key = keypad.getKey();

watchIdleSwitch();

if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe

if (key){ 
  
  resetIdleSwitch(); 
  
  switch(key){
    
    case '0':
    if((counter < 2) && (dockInt == 1)){
      setDock += 0;
      counter++;
    }
    else {mainBack++;}
      break;
    
    case '1':
    if((counter < 2) && (dockInt < 1)){
      setDock += 1;
      counter++;
    }
      break;
    
    case '2':
    if((counter < 2) && (dockInt < 1)){
      setDock += 2;
      counter++;
    }
      break;
    
    case '3':
    if((counter < 2) && (dockInt < 1)){
      setDock += 3;
      counter++;
    }
      break;
    
    case '4':
    if((counter < 2) && (dockInt < 1)){
      setDock += 4;
      counter++;
    }
      break;
    
    case '5':
    if((counter < 2) && (dockInt < 1)){
      setDock += 5;
      counter++;
    }
      break;
    
    case '6':
    if((counter < 2) && (dockInt < 1)){
      setDock += 6;
      counter++;
    }
      break;
    
    case '7':
    if((counter < 2) && (dockInt < 1)){
      setDock += 7;
      counter++;
    }
      break;
    
    case '8':
    if((counter < 2) && (dockInt < 1)){
      setDock += 8;
      counter++;
    }
      break;
    
    case '9':
    if((counter < 2) && (dockInt < 1)){
      setDock += 9;
      counter++;
    }
      break;
    
    case '*':
    if((counter <= 2) && (counter > 0)){
      counter = 0;
      mainBack = 0;
      i = false; 
    }
    
    case '#': //karakter törlés
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
  
  if      ((digitalRead(23) == HIGH) && (dockInt == 1)){newPassword();}
  else if ((digitalRead(23) == LOW)  && (dockInt == 1)){unlockDock();}
  else if ((digitalRead(25) == HIGH) && (dockInt == 2)){newPassword();}
  else if ((digitalRead(25) == LOW)  && (dockInt == 2)){unlockDock();}
  else if ((digitalRead(27) == HIGH) && (dockInt == 3)){newPassword();}
  else if ((digitalRead(27) == LOW)  && (dockInt == 3)){unlockDock();}
  else if ((digitalRead(29) == HIGH) && (dockInt == 4)){newPassword();}
  else if ((digitalRead(29) == LOW)  && (dockInt == 4)){unlockDock();}
  else if ((digitalRead(31) == HIGH) && (dockInt == 5)){newPassword();}
  else if ((digitalRead(31) == LOW)  && (dockInt == 5)){unlockDock();}
  else if ((digitalRead(33) == HIGH) && (dockInt == 6)){newPassword();}
  else if ((digitalRead(33) == LOW)  && (dockInt == 6)){unlockDock();}
  else if ((digitalRead(35) == HIGH) && (dockInt == 7)){newPassword();}
  else if ((digitalRead(35) == LOW)  && (dockInt == 7)){unlockDock();}
  else if ((digitalRead(37) == HIGH) && (dockInt == 8)){newPassword();}
  else if ((digitalRead(37) == LOW)  && (dockInt == 8)){unlockDock();}
  else if ((digitalRead(39) == HIGH) && (dockInt == 9)){newPassword();}
  else if ((digitalRead(39) == LOW)  && (dockInt == 9)){unlockDock();}
  else if ((digitalRead(41) == HIGH) && (dockInt == 10)){newPassword();}
  else if ((digitalRead(41) == LOW)  && (dockInt == 10)){unlockDock();}
  else{}   
}

//Zár feloldás------------------------------------------------------------------------
void unlockDock(){

setStar = "";

i = true;

if(language == 1){drawScreen(7);}
else if(language == 2){drawScreen(16);}

while(i){

key = keypad.getKey();

watchIdleSwitch();

if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe

if (key){ 

  resetIdleSwitch();

  dockInt = setDock.toInt();
  
  switch(key){
    
    case '0':
    if(counter < 4){
      secPassword += 0;
      setStar += '*';
      counter++;
    }
    else {mainBack++;}
      break;
    
    case '1':
    if(counter < 4){
      secPassword += 1;
      setStar += '*';
      counter++;
    }
      break;
    
    case '2':
    if(counter < 4){
      secPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    if(counter < 4){
      secPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    if(counter < 4){
      secPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    if(counter < 4){
      secPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    if(counter < 4){
      secPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    if(counter < 4){
      secPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    if(counter < 4){
      secPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    if(counter < 4){
      secPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case '*':
    if((counter <= 4) && (counter > 0)){

      if ((scanPassword(dockInt, secPassword) == true) && (language == 1)){
        drawScreen(11);
        delay(5000);
        unlockingClosedDock(); 
        counter = 0;
        mainBack = 0;
        i = false; 
      }
      else if ((scanPassword(dockInt, secPassword) == true) && (language == 2)){
        drawScreen(17);
        delay(5000);
        unlockingClosedDock();
        counter = 0;
        mainBack = 0;
        i = false;  
      }
      else if ((scanPassword(dockInt, secPassword) == false) && (language == 1)){
        drawScreen(10);        
        delay(5000);
        counter = 0;
        mainBack = 0;
        i = true;
        secPassword = "";
        unlockDock();  
      }
      else if ((scanPassword(dockInt, secPassword) == false) && (language == 2)){
        drawScreen(18);
        delay(5000);  
        counter = 0;
        mainBack = 0;
        i = true;
        secPassword = "";
        unlockDock();  
      }
      else{} 
    }
      break;
    
    case '#': //karakter törlés
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
  mainMenu();
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
      default:
        break;
    } 
}

//Új jelszó---------------------------------------------------------------------------
void newPassword(){

setStar = "";

i = true;

if(language == 1){drawScreen(8);}
else if(language == 2){drawScreen(19);}

while(i){

key = keypad.getKey();

watchIdleSwitch();

if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe

if (key){

  resetIdleSwitch();
  
  switch(key){
    
    case '0':
    if(counter < 4){
      setPassword += 0;
      setStar += '*';
      counter++;
    }
    else {mainBack++;}
      break;
    
    case '1':
    if(counter < 4){
      setPassword += 1;
      setStar += '*';
      counter++;
    }
      break;
    
    case '2':
    if(counter < 4){
      setPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    if(counter < 4){
      setPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    if(counter < 4){
      setPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    if(counter < 4){
      setPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    if(counter < 4){
      setPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    if(counter < 4){
      setPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    if(counter < 4){
      setPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    if(counter < 4){
      setPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case '*': //jelszó mentés
    if((counter <= 4) && (counter > 0)){
      counter = 0;
      mainBack = 0;
      i = false; 
    }
      break;
    
    case '#': //karakter törlés
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

if(language == 1){drawScreen(9);}
else if(language == 2){drawScreen(20);}

while(i){

key = keypad.getKey();

watchIdleSwitch();

if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe

if (key){ 

  resetIdleSwitch();
  
  switch(key){
    
    case '0':
    if(counter < 4){
      secPassword += 0;
      setStar += '*';
      counter++;
    }
    else {mainBack++;}
      break;
    
    case '1':
    if(counter < 4){
      secPassword += 1;
      setStar += '*';
      counter++;
    }
      break;
    
    case '2':
    if(counter < 4){
      secPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    if(counter < 4){
      secPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    if(counter < 4){
      secPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    if(counter < 4){
      secPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    if(counter < 4){
      secPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    if(counter < 4){
      secPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    if(counter < 4){
      secPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    if(counter < 4){
      secPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case '*': //jelszó mentés
    if((counter <= 4) && (counter > 0)){
      if(secPassword == setPassword) {
        if(language == 1)     {
          drawScreen(12);
          delay(5000);      //5 másodperc késleltetés a nyitás előtt
          unlockingOpenedDock();  //Nyitás, ha a megadott jelszavak egyeznek (magyar mód)
          }
        else if(language == 2){
          drawScreen(21);
          delay(5000);      //5 másodperc késleltetés a nyitás előtt
          unlockingOpenedDock();  //Nyitás, ha a megadott jelszavak egyeznek (angol mód)
          }
        counter = 0;
        mainBack = 0;
        i = false;
        }
      else if(secPassword != setPassword){
        if(language == 1)     {
          drawScreen(10);
          delay(3000);
          }
        else if(language == 2){
          drawScreen(18);
          delay(3000);
          }
        counter = 0;
        mainBack = 0;
        i = true;
        secPassword = "";
        againPassword();
        }  
    }
      break;
    
    case '#': //karakter törlés
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
  }
  mainMenu();
}

//Dokk feloldása------------------------------------------------------
void unlockingClosedDock(){

switch(dockInt){
    case 1:
      do{
      digitalWrite(A0, HIGH);
      }while(!(digitalRead(23)));
      digitalWrite(A0, LOW);
      break;
    case 2:
      do{
      digitalWrite(A1, HIGH);
      }while(!(digitalRead(25)));
      digitalWrite(A1, LOW);
      break;
    case 3:
      do{
      digitalWrite(A2, HIGH);
      }while(!(digitalRead(27)));
      digitalWrite(A2, LOW);
      break;
    case 4:
      do{
      digitalWrite(A3, HIGH);
      }while(!(digitalRead(29)));
      digitalWrite(A3, LOW);
      break;
    case 5:
      do{
      digitalWrite(A4, HIGH);
      }while(!(digitalRead(31)));
      digitalWrite(A4, LOW);
      break;
    case 6:
      do{
      digitalWrite(A5, HIGH);
      }while(!(digitalRead(33)));
      digitalWrite(A5, LOW);
      break;
    case 7:
      do{
      digitalWrite(A6, HIGH);
      }while(!(digitalRead(35)));
      digitalWrite(A6, LOW);
      break;
    case 8:
      do{
      digitalWrite(A7, HIGH);
      }while(!(digitalRead(37)));
      digitalWrite(A7, LOW);
      break;
    case 9:
      do{
      digitalWrite(A8, HIGH);
      }while(!(digitalRead(49)));
      digitalWrite(A8, LOW);
      break;
    case 10:
      do{
      digitalWrite(A9, HIGH);
      }while(!(digitalRead(41)));
      digitalWrite(A9, LOW);
      break;
  }
}

//Dokk feloldása a megerősítő jelszó után----------------------------------------
void unlockingOpenedDock(){

switch(dockInt){
    case 1:
      do{
      digitalWrite(A0, HIGH);
      }while(digitalRead(23));
      digitalWrite(A0, LOW);
      break;
    case 2:
      do{
      digitalWrite(A1, HIGH);
      }while(digitalRead(25));
      digitalWrite(A1, LOW);
      break;
    case 3:
      do{
      digitalWrite(A2, HIGH);
      }while(digitalRead(27));
      digitalWrite(A2, LOW);
      break;
    case 4:
      do{
      digitalWrite(A3, HIGH);
      }while(digitalRead(29));
      digitalWrite(A3, LOW);
      break;
    case 5:
      do{
      digitalWrite(A4, HIGH);
      }while(digitalRead(31));
      digitalWrite(A4, LOW);
      break;
    case 6:
      do{
      digitalWrite(A5, HIGH);
      }while(digitalRead(33));
      digitalWrite(A5, LOW);
      break;
    case 7:
      do{
      digitalWrite(A6, HIGH);
      }while(digitalRead(35));
      digitalWrite(A6, LOW);
      break;
    case 8:
      do{
      digitalWrite(A7, HIGH);
      }while(digitalRead(37));
      digitalWrite(A7, LOW);
      break;
    case 9:
      do{
      digitalWrite(A8, HIGH);
      }while(digitalRead(49));
      digitalWrite(A8, LOW);
      break;
    case 10:
      do{
      digitalWrite(A9, HIGH);
      }while(digitalRead(41));
      digitalWrite(A9, LOW);
      break;
  }
}

//Mester jelszó a feloldáshoz, elfelejtett kód esetén--------------------------------
void masterDockSelect(){

i = true;

drawScreen(13);

while(i){

key = keypad.getKey();

watchIdleSwitch();

if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe

if (key){ 
  
  resetIdleSwitch(); 

  dockInt = setDock.toInt();
  
  switch(key){
    
    case '0':
    if((counter < 2) && (dockInt == 1)){
      setDock += 0;
      counter++;
    }
    else {mainBack++;}
      break;
    
    case '1':
    if((counter < 2) && (dockInt < 1)){
      setDock += 1;
      counter++;
    }
      break;
    
    case '2':
    if((counter < 2) && (dockInt < 1)){
      setDock += 2;
      counter++;
    }
      break;
    
    case '3':
    if((counter < 2) && (dockInt < 1)){
      setDock += 3;
      counter++;
    }
      break;
    
    case '4':
    if((counter < 2) && (dockInt < 1)){
      setDock += 4;
      counter++;
    }
      break;
    
    case '5':
    if((counter < 2) && (dockInt < 1)){
      setDock += 5;
      counter++;
    }
      break;
    
    case '6':
    if((counter < 2) && (dockInt < 1)){
      setDock += 6;
      counter++;
    }
      break;
    
    case '7':
    if((counter < 2) && (dockInt < 1)){
      setDock += 7;
      counter++;
    }
      break;
    
    case '8':
    if((counter < 2) && (dockInt < 1)){
      setDock += 8;
      counter++;
    }
      break;
    
    case '9':
    if((counter < 2) && (dockInt < 1)){
      setDock += 9;
      counter++;
    }
      break;
    
    case '*':
    if((counter <= 2) && (counter > 0)){
      counter = 0;
      mainBack = 0;
      i = false; 
    }
    
    case '#': //karakter törlés
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

if(mainBack > 1){mainMenu();} //két 0 gombnyomásra vissza a főmenübe

if (key){ 

  resetIdleSwitch();
  
  switch(key){
    
    case '0':
    if(counter < 6){
      secPassword += 0;
      setStar += '*';
      counter++;
    }
    else {mainBack++;}
      break;
    
    case '1':
    if(counter < 6){
      secPassword += 1;
      setStar += '*';
      counter++;
    }
      break;
    
    case '2':
    if(counter < 6){
      secPassword += 2;
      setStar += '*';
      counter++;
    }
      break;
    
    case '3':
    if(counter < 6){
      secPassword += 3;
      setStar += '*';
      counter++;
    }
      break;
    
    case '4':
    if(counter < 6){
      secPassword += 4;
      setStar += '*';
      counter++;
    }
      break;
    
    case '5':
    if(counter < 6){
      secPassword += 5;
      setStar += '*';
      counter++;
    }
      break;
    
    case '6':
    if(counter < 6){
      secPassword += 6;
      setStar += '*';
      counter++;
    }
      break;
    
    case '7':
    if(counter < 6){
      secPassword += 7;
      setStar += '*';
      counter++;
    }
      break;
    
    case '8':
    if(counter < 6){
      secPassword += 8;
      setStar += '*';
      counter++;
    }
      break;
    
    case '9':
    if(counter < 6){
      secPassword += 9;
      setStar += '*';
      counter++;
    }
      break;
    
    case '*':
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
    
    case '#': //karakter törlés
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
    u8g.drawStr( 0, 30, "   * Magyar Nyelv *  ");
    u8g.drawStr( 0, 41, " # English Language #");
    break;
    case 6:
    u8g.drawStr( 0, 30,  " \xcd""rd be a feloldani");
    u8g.drawStr( 0, 41, " k""\xed""v""\xe1""nt"" dokk sz""\xe1""m""\xe1""t""!");
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
    u8g.drawStr( 0, 12,  "********Admin********");
    u8g.drawStr( 0, 25, "   K""\xe9""rem adja meg a   ");
    u8g.drawStr( 0, 36, " felold""\xe1""shoz az admin");
    u8g.drawStr( 0, 47, "       jelsz""\xf3""t!       ");
    break;
//Angol-------------------------------------------------------------------------
    case 15:
    u8g.drawStr( 0, 30, " Enter dock's number ");
    u8g.drawStr( 0, 41, " you want to unlock! ");
    break;
    case 16:
    u8g.drawStr( 0, 30, "  Please enter your  ");
    u8g.drawStr( 0, 41, " PIN code to unlock! ");
    break;
    case 17:
    u8g.drawStr( 0, 8,  "  Super, your bike  ");
    u8g.drawStr( 0, 19, " parking process is ");
    u8g.drawStr( 0, 30, "over. Please take it");
    u8g.drawStr( 0, 41, " out of the dock and");
    u8g.drawStr( 0, 52, "leave the gate open!");
    u8g.drawStr( 0, 63, "  Have a nice trip! "); 
    break;
    case 18:
    u8g.drawStr( 0, 21, " Oops, it looks like");
    u8g.drawStr( 0, 32, "  you entered the   ");
    u8g.drawStr( 0, 43, "     wrong code.    ");
    u8g.drawStr( 0, 54, "  Please try again! ");  
    break;
    case 19:
    u8g.drawStr( 0, 14,  "   Please enter a   ");
    u8g.drawStr( 0, 25, "  4-digit PIN code! ");
    u8g.drawStr( 0, 36, " After parking, you ");
    u8g.drawStr( 0, 47, "will be able to open");
    u8g.drawStr( 0, 58, " the dock with this.");
    break;
    case 20:
    u8g.drawStr( 0, 30, "  Please enter the  ");
    u8g.drawStr( 0, 41, "     PIN again!     ");
    break;
    case 21:
    u8g.drawStr( 0, 21,  " Ready! Please place"); 
    u8g.drawStr( 0, 32, " the bicycle in the "); 
    u8g.setPrintPos( 20, 43);
    u8g.print(setDock);
    u8g.print('.');
    u8g.drawStr( 35, 43,"storage and   ");
    u8g.drawStr( 0, 54, "   close the gate!  "); 
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
