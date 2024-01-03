/***********************************************************/
//  Program made by Declan Ross
//  26,08,2018
//  V3.1
//
//
//    MP3:
//    sendSong(CMD_PLAY_W_VOL, 0x1E, 0x02);     Replace 0x02 with Hex MP3 name, use this for playing files
//                                              Also, 0x1E can be replaced with a volume Hex number (0-30)
//                                              0x1E=30
//
//
//    ammoQuota = checkAmmo(wpSL);              Use these 2 lines of code to refresh the display
//    lcdPrintPL(hpSL, ammoLeft, wpSL, tmSL);
//
//    ammoLeft=ammoQuota;                       Use this one to reload
//
//    Use pause() instead of delay() so that interupts will work
//
//
//    USE THE MAIN DIRECTORY FOR IMPORTANT TIMING ON THE SD CARD
//    MP3's IN A FOLDER TAKE LONGER TO EXECUTE
//
//
//    Use function pauseIR() instead of delays so that the arduino can recieve singals
//    and pause at the same time
//
//    pauseIR(500, tmSL, hpSL);
//
//    //USE THIS LINE TO CHECK IRIN SIGNAL
//    checkIRIN(tmSL, hpSL);
//
//
//
//
//
/***********************************************************/

/***********************************************************/
//For easy customisation change values
//
//
#define m4dmg 35
#define smgdmg 21
#define uspdmg 40
#define awpdmg 75
#define m4sdmg 30
#define smgsdmg 19
#define uspsdmg 38
#define awpsdmg 70

//MP3 custom lib
static int8_t Send_buf[8] = {0} ;
#define CMD_PLAY_W_INDEX 0X03
#define CMD_SET_VOLUME 0X06
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_SINGLE_CYCLE 0X19
#define SINGLE_CYCLE_ON 0X00
#define SINGLE_CYCLE_OFF 0X01
#define CMD_PLAY_W_VOL 0X22
#define CMD_PLAY_IN_FOLDER 0X0F

//Include The folowing Libraries
//LCD req
#include <LiquidCrystal_I2C.h>
//IR req
//Experimental
#include <IRremote.h>
//Working
//#include <IRLibDecodeBase.h>
//#include <IRLibSendBase.h>
//#include <IRLib_P01_NEC.h>
//#include <IRLib_P02_Sony.h>
//#include <IRLib_HashRaw.h>  //Must be last protocol
//#include <IRLibCombo.h>
//#include <IRLibRecv.h>
//MP3 Req
#include <SoftwareSerial.h>
#include <Wire.h>

//Define logical pins to words
#define fireTrigger 7
#define irIn 2
#define magReload 4
#define vMotor 5
#define lowHP 6
#define irOut 7
#define ARDUINO_RX A6
#define ARDUINO_TX 8
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);
#define tmBluLED 9
#define tmRedLED 10
//#define fireMode 11
#define slideSW 12
#define hpSW A7
#define ST1LED 17
#define ST2LED 16
#define tmSW 15
#define wpSW 14
#define confSW 13

//Millis timer functionality
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;


//Set integers
int setConfirmed;
int displayCountdown;
boolean alive;
int timerBreak;
unsigned long pauseT;
unsigned long gunSound;
unsigned long hexCode;
int pauseTime;
boolean slideDone;
boolean oof;
int dmgM;
long unsigned longValue;

//Set button state variables
boolean rlST;
boolean sldST;
boolean trST;
boolean tmST;
boolean wpST;
int hpST;
boolean confST;
boolean slideST;
boolean btnPressed;

//Set selected state veriables
int tmSL;
int wpSL = 0;
int hpSL; //This also doubles as current hp

//Weapon variables
int ammoQuota;
int ammoLeft;

//Define others
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
IRrecv irrecv(2);
decode_results results;
IRsend irsend;

//IRrecv myReceiver(2);
//IRdecode myDecoder;
//IRsend mySender;


void setup() {
  startMillis = millis();
  pinMode(tmBluLED, OUTPUT);
  pinMode(tmRedLED, OUTPUT);   // start serial interface
  pinMode(ST1LED, OUTPUT);
  pinMode(ST2LED, OUTPUT);
  pinMode(vMotor, OUTPUT);
  pinMode(lowHP, OUTPUT);
  mySerial.begin(9600);
  pause(500);             // wait for init
  selectCommand(CMD_SEL_DEV, DEV_TF);   //select sd-card
  pause(500);         // wait for init
  lcd.begin(16, 2);
  pause(200);
  lcd.print("LAZERTAG beta3.1");
  lcd.setCursor(0, 1);
  lcd.print("Made by:Declan.R");
  pause(1000);
  lcd.clear();
  lcd.print("HP:     Setup   ");
  lcd.setCursor(0, 1);
  lcd.print("WP:     Team:   ");
  randomSeed(analogRead(6));
}

void loop() {
  while (setConfirmed == 0) {  //Setup code for setting variables, player cannot be hit and this should freeze when player clicks confirm.
    wpSL = wpSLfunc(wpSL);
    hpSL = hpSLfunc(hpSL);
    tmSL = tmSLfunc(tmSL);
    setConfirmed = confunc(setConfirmed);
  }

  if (displayCountdown == 0) {
    lcd.clear();
    lcd.print("Game starts in:");
    lcd.setCursor(7, 1);
    pause(200);
    lcd.print("3");
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x05);
    pause(400);
    lcd.setCursor(7, 1);
    lcd.print("2");
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x05);
    pause(400);
    lcd.setCursor(7, 1);
    lcd.print("1");
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x05);
    pause(400);
    lcd.setCursor(7, 1);
    lcd.print("GO!");
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x06);
    pause(400);
    displayCountdown = 1;
    ammoQuota = checkAmmo(wpSL);              //Use these 2 lines of code to refresh the screen
    ammoLeft = ammoQuota;
    lcdPrintPL(hpSL, ammoLeft, wpSL, tmSL);     //with the HP, TM, current AMMO and WP
    irrecv.enableIRIn();
  }
  while (alive == 0) {
    do {
      if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
        hpSL = checkIR(tmSL, hpSL, results.value);
        results.value = 0x00;
        if (hpSL == 0 or hpSL <= 0) {
          sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
          deadLoop();
        }
        lcdUpdateHP(hpSL);
      }
      ammoQuota = checkAmmo(wpSL);
      trST = digitalRead(fireTrigger);
      if (trST == 1) {
        btnPressed = 1;
      }
      wpST = digitalRead(wpSW);
      if (wpST == 1) {
        btnPressed = 1;
      }
      rlST = digitalRead(magReload);
      if (rlST == 1) {
        btnPressed = 1;
      }
    } while (btnPressed == 0);
    if (wpST == 1) {
      wpSL = wpSLfunc2(wpSL);
      lcdPrintPL(hpSL, ammoLeft, wpSL, tmSL);
      ammoLeft = 0;
      lcd.setCursor(8, 0);
      lcd.print("RELOAD  ");
      btnPressed = 0;
    }
    if (rlST == 1) {
      lcd.setCursor(8, 0);
      lcd.print("RELOAD  ");
      lcd.setCursor(0, 1);
      lcd.print("Reloading       ");
      sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x07);   //play mag reload sound
      pause(70);
      digitalWrite(vMotor, HIGH);
      pause(150);
      if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
        hpSL = checkIR(tmSL, hpSL, results.value);
        results.value = 0x00;
        if (hpSL == 0 or hpSL <= 0) {
          sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
          deadLoop();
        }
        lcdUpdateHP(hpSL);
      }
      digitalWrite(vMotor, LOW);
      pause(530);
      if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
        hpSL = checkIR(tmSL, hpSL, results.value);
        results.value = 0x00;
        if (hpSL == 0 or hpSL <= 0) {
          sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
          deadLoop();
        }
        lcdUpdateHP(hpSL);
      }
      digitalWrite(vMotor, HIGH);
      pause(150);
      if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
        hpSL = checkIR(tmSL, hpSL, results.value);
        results.value = 0x00;
        if (hpSL == 0 or hpSL <= 0) {
          sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
          deadLoop();
        }
        lcdUpdateHP(hpSL);
      }
      digitalWrite(vMotor, LOW);
      pause(500);
      if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
        hpSL = checkIR(tmSL, hpSL, results.value);
        results.value = 0x00;
        if (hpSL == 0 or hpSL <= 0) {
          sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
          deadLoop();
        }
        lcdUpdateHP(hpSL);
      }
      slideDone = 0;
      while (slideDone == 0) {
        lcd.setCursor(0, 1);
        lcd.print("Slide to reload ");
        slideST = digitalRead(slideSW);
        if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
          hpSL = checkIR(tmSL, hpSL, results.value);
          results.value = 0x00;
          if (hpSL == 0 or hpSL <= 0) {
            sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
            deadLoop();
          }
          lcdUpdateHP(hpSL);
        }
        if (slideST == 1) {
          sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x08);
          pause(50);
          lcdUpdateHP(hpSL);
          digitalWrite(vMotor, HIGH);
          pause(100);
          lcdUpdateHP(hpSL);
          digitalWrite(vMotor, LOW);
          pause(200);
          digitalWrite(vMotor, HIGH);
          pause(100);
          digitalWrite(vMotor, LOW);
          pause(200);
          lcdUpdateHP(hpSL);
          ammoLeft = ammoQuota;
          lcdPrintPL(hpSL, ammoLeft, wpSL, tmSL);
          slideDone = 1;
        }
      }
      btnPressed = 0;
    }
    if (trST == 1) { //Firing mechanism
      if (ammoLeft >= 1) {
        gunSound = sendMP3(wpSL);
        sendSong(CMD_PLAY_IN_FOLDER, 0x04, gunSound);   //use a search function here to find the mp3 name
        digitalWrite(ST1LED, HIGH);
        ammoLeft = ammoLeft - 1;
        lcdUpdate(ammoLeft);
        lcdUpdateHP(hpSL);
        if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
          hpSL = checkIR(tmSL, hpSL, results.value);
          results.value = 0x00;
          if (hpSL == 0 or hpSL <= 0) {
            sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
            deadLoop();
          }
          lcdUpdateHP(hpSL);
        }
        digitalWrite(vMotor, HIGH);
        pauseTime = frCheck(wpSL);
        hexCode = sendIRCODE(tmSL, wpSL);
        irsend.sendNEC(hexCode, 16);
        irrecv.enableIRIn();
        irrecv.resume();
        pause(pauseTime);
        digitalWrite(lowHP, LOW);
        digitalWrite(vMotor, LOW);
        if (irrecv.decode(&results)) {        /// ADD THIS ItF STATEMENT TO OTHER IR CHECKERS
          hpSL = checkIR(tmSL, hpSL, results.value);
          results.value = 0x00;
          if (hpSL == 0 or hpSL <= 0) {
            sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
            deadLoop();
          }
          lcdUpdateHP(hpSL);
        }
        digitalWrite(vMotor, LOW);
        digitalWrite(ST1LED, LOW);
        if (wpSL == awpdmg or wpSL == awpsdmg) {
          pause(pauseTime);
          lcd.setCursor(0, 1);
          lcd.print("Slide to load   ");
          digitalWrite(ST1LED, LOW);
          slideST = digitalRead(slideSW);
          while (slideST == 0) {
            if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
              hpSL = checkIR(tmSL, hpSL, results.value);
              results.value = 0x00;
              if (hpSL == 0 or hpSL <= 0) {
                sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
                deadLoop();
              }
              lcdUpdateHP(hpSL);
            }
            slideST = digitalRead(slideSW);
          }
          sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x08);
          lcdPrintPL(hpSL, ammoLeft, wpSL, tmSL);
          lcdUpdateHP(hpSL);
        }
        else if (wpSL == uspdmg or wpSL == uspsdmg) {
          digitalWrite(ST1LED, LOW);
          trST = digitalRead(fireTrigger);
          while (trST == 1) {
            trST = digitalRead(fireTrigger);
            if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
              hpSL = checkIR(tmSL, hpSL, results.value);
              results.value = 0x00;
              if (hpSL == 0 or hpSL <= 0) {
                sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
                deadLoop();
              }
              lcdUpdateHP(hpSL);
            }
          }
        }
        else {
          if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
            hpSL = checkIR(tmSL, hpSL, results.value);
            results.value = 0x00;
            if (hpSL == 0 or hpSL <= 0) {
              sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
              deadLoop();
            }
            lcdUpdateHP(hpSL);
          }
        }


        //
      }
      else if (ammoLeft == 0) {
        sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x01);
        lcd.setCursor(8, 0);
        lcd.print("RELOAD  ");
        pauseTime = frCheck(wpSL);
        pause(pauseTime);
        if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
          hpSL = checkIR(tmSL, hpSL, results.value);
          results.value = 0x00;
          if (hpSL == 0 or hpSL <= 0) {
            sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
            deadLoop();
          }
          lcdUpdateHP(hpSL);
        }
        pause(220);
        if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
          hpSL = checkIR(tmSL, hpSL, results.value);
          results.value = 0x00;
          if (hpSL == 0 or hpSL <= 0) {
            sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
            deadLoop();
          }
          lcdUpdateHP(hpSL);
        }//to simulate the delay of the mySend.send(); command
        trST = digitalRead(fireTrigger);

      }
      btnPressed = 0;
    }
  }//Main code can start here as the setup statement will now be completly ignored
}


int hpSLfunc (int hp) { //HP SELECT FUNCTION
  hpST = analogRead(hpSW);
  if (hp == 0) {
    hp = 100;
    lcd.setCursor(3, 0);
    lcd.print("100");
    return hp;
  }
  if (hpST >= (900)) {
    lcd.setCursor(3, 0);
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x03);
    if (hp == 1) {
      hp = 100;
      lcd.print("100");
      pause(500);
      return hp;
    }
    if (hp == 100) {
      hp = 75;
      lcd.print("75 ");
      pause(500);
      return hp;
    }
    if (hp == 75) {
      hp = 50;
      lcd.print("50 ");
      pause(500);
      return hp;
    }
    if (hp == 50) {
      hp = 25;
      lcd.print("25 ");
      pause(500);
      return hp;
    }
    if (hp == 25) {
      hp = 1;
      lcd.print("1  ");
      pause(500);
      return hp;
    }
  }
}

int confunc(int c) { //CONFIRM SW FUNCTION
  confST = digitalRead(confSW);
  if (confST == 1) {
    c = 1;
    return c;
  }
  if (confST == 0) {
    c = 0;
    return c;
  }
}

int wpSLfunc(int wp) { //WEAPON SW FUNCTION
  wpST = digitalRead(wpSW);
  if (wp == 0) {
    lcd.setCursor(3, 1);
    lcd.print("M4  ");
    wp = m4dmg;
    return wp;
  }
  if (wpST == 1) {
    lcd.setCursor(3, 1);
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x02);
    if (wp == awpsdmg) {
      wp = m4dmg;
      lcd.print("M4  ");
      pause(500);
      return wp;
    }
    if (wp == m4dmg) {
      wp = smgdmg;
      lcd.print("SMG ");
      pause(500);
      return wp;
    }
    if (wp == smgdmg) {
      wp = uspdmg;
      lcd.print("USP ");
      pause(500);
      return wp;
    }
    if (wp == uspdmg) {
      wp = awpdmg;
      lcd.print("AWP ");
      pause(500);
      return wp;
    }
    if (wp == awpdmg) {
      wp = m4sdmg;
      lcd.print("M4s ");
      pause(500);
      return wp;
    }
    if (wp == m4sdmg) {
      wp = smgsdmg;
      lcd.print("SMGs");
      pause(500);
      return wp;
    }
    if (wp == smgsdmg) {
      wp = uspsdmg;
      lcd.print("USPs");
      pause(500);
      return wp;
    }
    if (wp == uspsdmg) {
      wp = awpsdmg;
      lcd.print("AWPs");
      pause(500);
      return wp;
    }
  }
}

int tmSLfunc(int tm) {
  tmST = digitalRead(tmSW);
  if (tm == 0) {
    tm = 2;
    lcd.setCursor(13, 1);
    lcd.print("RED");
    digitalWrite(tmRedLED, HIGH);
    digitalWrite(tmBluLED, LOW);
    pause(500);
    return tm;
  }
  if (tmST == 1) {
    sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x04);
    lcd.setCursor(13, 1);
    if (tm == 3) {
      lcd.print("BLU");
      tm = 1;
      digitalWrite(tmRedLED, LOW);
      digitalWrite(tmBluLED, HIGH);
      pause(500);
      return tm;
    }
    if (tm == 1) {
      tm = 2;
      lcd.print("RED");
      digitalWrite(tmRedLED, HIGH);
      digitalWrite(tmBluLED, LOW);
      pause(500);
      return tm;
    }
    if (tm == 2) {
      tm = 3;
      lcd.print("FFA");
      digitalWrite(tmRedLED, HIGH);
      digitalWrite(tmBluLED, HIGH);
      pause(500);
      return tm;
    }
  }
}

void lcdPrintPL (int hp, int am, int wp, int tm) {
  lcd.clear();
  lcd.print("HP:    ");
  lcd.setCursor(3, 0);
  lcd.print(hp);
  lcd.setCursor(8, 0);
  lcd.print("Ammo:");
  lcd.print(am);
  lcd.setCursor(0, 1);
  lcd.print("WP:");
  if (wp == m4dmg) {
    lcd.print("M4");
  }
  if (wp == smgdmg) {
    lcd.print("SMG");
  }
  if (wp == uspdmg) {
    lcd.print("USP");
  }
  if (wp == awpdmg) {
    lcd.print("AWP");
  }
  if (wp == m4sdmg) {
    lcd.print("M4s");
  }
  if (wp == smgsdmg) {
    lcd.print("SMGs");
  }
  if (wp == uspsdmg) {
    lcd.print("USPs");
  }
  if (wp == awpsdmg) {
    lcd.print("AWPs");
  }
  lcd.setCursor(8, 1);
  lcd.print("Team:");
  if (tm == 2) {
    lcd.print("RED");
  }
  if (tm == 1) {
    lcd.print("BLU");
  }
  if (tm == 3) {
    lcd.print("FFA");
  }
}

int checkAmmo (int wp) {
  int am;
  if (wp == m4dmg) {
    am = 31;
    return am;
  }
  if (wp == smgdmg) {
    am = 41;
    return am;
  }
  if (wp == uspdmg) {
    am = 16;
    return am;
  }
  if (wp == awpdmg) {
    am = 6;
    return am;
  }
  if (wp == m4sdmg) {
    am = 31;
    return am;
  }
  if (wp == smgsdmg) {
    am = 41;
    return am;
  }
  if (wp == uspsdmg) {
    am = 16;
    return am;
  }
  if (wp == awpsdmg) {
    am = 6;
    return am;
  }
}

int selectCommand(int8_t command, int16_t dat)
{
  pause(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = 0x01; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for (uint8_t i = 0; i < 8; i++) //
  {
    mySerial.write(Send_buf[i]) ;
  }
}

int sendSong(int8_t command, int16_t dat, int16_t dat2)
{
  pause(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (dat);//datah
  Send_buf[6] = (dat2); //datal
  Send_buf[7] = 0xef; //ending byte
  for (uint8_t i = 0; i < 8; i++) //
  {
    mySerial.write(Send_buf[i]) ;
  }
}

void pause(unsigned long period) { //pause() replacing delay() function by using a simple millis() function
  do {
    currentMillis = millis();
    //add a function here that repeatabily checks the if there is an IR signal in, and if there is (and confirmed == 1) and it is not corrupt
    //then goto a function that checks the team and if it a different team subract Hp and display onscreen
    if (currentMillis - startMillis >= period)
    {
      startMillis = currentMillis;
      return;
    }
  } while (timerBreak == 0);
}

int wpSLfunc2(int wp) { //WEAPON SW FUNCTION
  sendSong(CMD_PLAY_IN_FOLDER, 0x02, 0x02);
  if (wp == awpsdmg) {
    wp = m4dmg;
    pause(200);
    return wp;
  }
  if (wp == m4dmg) {
    wp = smgdmg;
    pause(200);
    return wp;
  }
  if (wp == smgdmg) {
    wp = uspdmg;
    pause(200);
    return wp;
  }
  if (wp == uspdmg) {
    wp = awpdmg;
    pause(200);
    return wp;
  }
  if (wp == awpdmg) {
    wp = m4sdmg;
    pause(200);
    return wp;
  }
  if (wp == m4sdmg) {
    wp = smgsdmg;
    pause(200);
    return wp;
  }
  if (wp == smgsdmg) {
    wp = uspsdmg;
    pause(200);
    return wp;
  }
  if (wp == uspsdmg) {
    wp = awpsdmg;
    pause(200);
    return wp;
  }
}

unsigned long sendIRCODE(int tm, int wp) {       //Send signal about shot data
  if (tm == 3) {                      //FFA team wp check
    if (wp == m4dmg) {
      return 0x1F;
    }
    if (wp == smgdmg) {
      return 0x20;
    }
    if (wp == uspdmg) {
      return 0x21;
    }
    if (wp == awpdmg) {
      return 0x22;
    }
    if (wp == m4sdmg) {
      return 0x23;
    }
    if (wp == smgsdmg) {
      return 0x24;
    }
    if (wp == uspsdmg) {
      return 0x25;
    }
    if (wp == awpsdmg) {
      return 0x26;
    }
  }
  if (tm == 1) {                      //BLU team wp check
    if (wp == m4dmg) {
      return 0x29;
    }
    if (wp == smgdmg) {
      return 0x2A;
    }
    if (wp == uspdmg) {
      return 0x2B;
    }
    if (wp == awpdmg) {
      return 0x2C;
    }
    if (wp == m4sdmg) {
      return 0x2D;
    }
    if (wp == smgsdmg) {
      return 0x2E;
    }
    if (wp == uspsdmg) {
      return 0x2F;
    }
    if (wp == awpsdmg) {
      return 0x30;
    }
  }
  if (tm == 2) {                      //RED team wp check
    if (wp == m4dmg) {
      return 0x33;
    }
    if (wp == smgdmg) {
      return 0x34;
    }
    if (wp == uspdmg) {
      return 0x35;
    }
    if (wp == awpdmg) {
      return 0x36;
    }
    if (wp == m4sdmg) {
      return 0x37;
    }
    if (wp == smgsdmg) {
      return 0x38;
    }
    if (wp == uspsdmg) {
      return 0x39;
    }
    if (wp == awpsdmg) {
      return 0x3A;
    }
  }
}

unsigned long sendMP3(int wp) {       //Send MP3 hex sound file name
  if (wp == m4dmg) {
    return 0x04;
  }
  if (wp == smgdmg) {
    return 0x01;
  }
  if (wp == uspdmg) {
    return 0x03;
  }
  if (wp == awpdmg) {
    return 0x05;
  }
  if (wp == m4sdmg) {
    return 0x02;
  }
  if (wp == smgsdmg) {
    return 0x02;
  }
  if (wp == uspsdmg) {
    return 0x02;
  }
  if (wp == awpsdmg) {
    return 0x02;
  }

}

int frCheck(int wp) {
  int fr;
  if (wp == m4dmg or wp == m4sdmg) {
    fr = random(70, 110);
    return fr;
  }
  if (wp == smgdmg or wp == smgsdmg) {
    fr = random(40, 70);
    return fr;
  }
  if (wp == uspdmg or wp == uspsdmg) {
    return 100;
  }
  if (wp == awpdmg or wp == awpsdmg) {
    //execute function here for timing
    return 1000 ;
  }
}

void lcdUpdate(int am) {
  lcd.setCursor(8, 0);
  lcd.print("Ammo:  ");
  lcd.setCursor(13, 0);
  lcd.print(am);
}

void lcdUpdateHP(int hp) {
  lcd.setCursor(0, 0);
  lcd.print("HP:    ");
  lcd.setCursor(3, 0);
  lcd.print(hp);
}

int checkIR(int tm, int hp, unsigned long res) {
  int dmg;
  longValue = res, HEX;
  digitalWrite(lowHP, HIGH);
  if (longValue == 0x1F) {         //FFA team wp check
    hp = hp - m4dmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x20) {
    hp = hp - smgdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x21) {
    hp = hp - smgdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x22) {
    hp = hp - awpdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x23) {
    hp = hp - m4sdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x24) {
    hp = hp - smgsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x25) {
    hp = hp - uspsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  else if (longValue == 0x26) {
    hp = hp - awpsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    irrecv.resume();
    return hp;
  }
  //BLU team wp check

  else if (longValue == 0x29) {    //BLU

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - m4dmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x2A) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - smgdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x2B) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - uspdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x2C) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - awpdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x2D) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - m4sdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x2E) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - smgsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x2F) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - uspsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x30) {

    irrecv.resume();
    if (tm == 1) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - awpsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  //RED team wp check
  else if (longValue == 0x33) {     //RED

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - m4dmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x34) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - smgdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x35) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - smgdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x36) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - awpdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x37) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - m4sdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x38) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - smgsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x39) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - uspsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else if (longValue == 0x3A) {

    irrecv.resume();
    if (tm == 2) {
      digitalWrite(lowHP, LOW);
      return hp;
    }
    hp = hp - awpsdmg;
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x03);
    return hp;
  }
  else {
    irrecv.resume();
    digitalWrite(lowHP, LOW);
    return hp;
  }
}




void deadLoop() {
  digitalWrite(vMotor, LOW);
  digitalWrite(lowHP, HIGH);
  digitalWrite(ST1LED, HIGH);
  lcd.clear();
  lcd.print("GGWP!");
  lcd.setCursor(0, 1);
  lcd.print("You are dead ):");
  do {
    digitalWrite(tmRedLED, HIGH);
    digitalWrite(tmBluLED, HIGH);
    pause(300);
    digitalWrite(tmRedLED, LOW);
    digitalWrite(tmBluLED, LOW);
    pause(300);
  } while (oof == 0);
}


/*void checkIRIN (int tm, int hp) {
  if (irrecv.decode(&results)) {        /// ADD THIS IF STATEMENT TO OTHER IR CHECKERS
    hp = checkIR(tm, hp);
    irrecv.resume();
    sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
    if (hp == 0 or hp <= 0) {
      sendSong(CMD_PLAY_IN_FOLDER, 0x03, 0x02);
      deadLoop();
    }
    lcdUpdateHP(hpSL);
  }
  }*/

