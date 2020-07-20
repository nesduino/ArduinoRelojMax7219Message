// Reloj con segundero visualizado en Matrix de LED Max7219 cargado en Arduino Uno
// Primer versión del proyecto

// Barracas 20/07/2020 - Kern Néstor

// Program to demonstrate the MD_Parola library
//
// Display the time in one zone and other information scrolling through in
// another zone.
// - Time is shown in a user defined fixed width font
// - Scrolling text uses the default font
// - Temperature display uses user defined characters
// - Optional use of DS1307 module for time and DHT11 sensor for temp and humidity
// - DS1307 library (MD_DS1307) found at https://github.com/MajicDesigns/DS1307
// - DHT11 library (DHT11_lib) found at https://github.com/mathworks/thingspeak-arduino
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#define USE_DS1307

// Header file includes
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include <MD_DS1307.h>
#include <SimpleDHT.h>
#include "Font_Data.h"

// DHT config.
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);




volatile boolean buttonA = false;
volatile boolean buttonB = false;
volatile boolean buttonC = false;
int StateOfbuttonA = 0;
int StateOfbuttonB = 0;  
int StateOfbuttonC = 0;        
int NewStateOfbuttonA = 0;        
int NewStateOfbuttonB = 0;        
int NewStateOfbuttonC = 0;  
int Mode = 0;
int contrast = 0;
int SPEED_TIME = 75;

#define PAUSE_TIME  0

#define MAX_MESG  20

// Global variables
char szTime[9];    // hh:mm
char szsecond[4];    // ss
char szMesg[MAX_MESG+1] = "";

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Deg C

char *mon2str(uint8_t mon, char *psz, uint8_t len)

// Get a label from PROGMEM into a char array
{
  static const __FlashStringHelper* str[] =
  {
    F("Jan"), F("Feb"), F("Mar"), F("Apr"),
    F("May"), F("Jun"), F("Jul"), F("Aug"),
    F("Sep"), F("Oct"), F("Nov"), F("Dec")
  };

  strncpy_P(psz, (const char PROGMEM *)str[mon-1], len);
  psz[len] = '\0';

  return(psz);
}

char *dow2str(uint8_t code, char *psz, uint8_t len)
{
  static const __FlashStringHelper*  str[] =
  {
    F("Sunday"), F("Monday"), F("Tuesday"),F("Wednesday"), 
    F("Thursday"), F("Friday"),
    F("Saturday")
  };

  strncpy_P(psz, (const char PROGMEM *)str[code-1], len);
  psz[len] = '\0';

  return(psz);
}

void getTime(char *psz, bool f = true)
// Code for reading clock time
{
   RTC.readTime();
  sprintf(psz, "%02d%c%02d", RTC.h, (f ? ':' : ' '), RTC.m);
}
void getTim(char *psz, bool f = true)
// Code for reading clock time
{
   RTC.readTime();
  sprintf(psz, "%02d%c%02d", RTC.h, ':', RTC.m);
}

void getDate(char *psz)
// Code for reading clock date
{
  char  szBuf[10];

  RTC.readTime();
  sprintf(psz, "%d %s %04d", RTC.dd, mon2str(RTC.mm, szBuf, sizeof(szBuf)-1), RTC.yyyy);

}
void getTem(char *psz, bool f = true)
// Code for reading clock date
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    delay(100);
    return;
  }
  sprintf(psz, "%02d", (int)temperature);

}
void getHumi(char *psz, bool f = true)
// Code for reading clock date
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    delay(100);
    return;
  }
  sprintf(psz, "%02d", (int)humidity);

}

void getsecond(char *psz)
// Code for reading clock date
{
  char  szBuf[10];

  RTC.readTime();
  sprintf(psz, "%02d", RTC.s);
}


void gethh(char *psz, bool f = true)
// Code for reading clock time
{
   RTC.readTime();
  sprintf(psz, "%c%02d%c%02d", (f ? ':' : ' '), RTC.h, (f ? ':' : ' '), RTC.m);
}


void getmin(char *psz, bool f = true)
// Code for reading clock time
{
   RTC.readTime();
  sprintf(psz, "%02d%c%02d%c", RTC.h, (f ? ':' : ' '), RTC.m, (f ? ':' : ' '));
}

void getsec(char *psz)
// Code for reading clock date
{
  char  szBuf[10];

  RTC.readTime();
  sprintf(psz, "%02d", RTC.s);
}

void getdyy(char *psz)
{
  char  szBuf[10];

  RTC.readTime();
  sprintf(psz, "%02d", RTC.dd);

}

void getmon(char *psz)
// Code for reading clock date
{
  char  szBuf[10];

  RTC.readTime();
  sprintf(psz, "%s", mon2str(RTC.mm, szBuf, sizeof(szBuf)-1));

}

void getyyyy(char *psz)
// Code for reading clock date
{
  char  szBuf[10];

  RTC.readTime();
  sprintf(psz, "%04d", RTC.yyyy);

}


void setup(void)
{
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  P.begin(3);
  P.setInvert(false);
  P.setZone(2, 0, 3);
  P.setZone(1, 1, 3);
  P.setZone(0, 4, 1);
  P.setFont(1, numeric7Se);
  P.setFont(0, numeric7Seg);
  P.displayZoneText(1, szTime, PA_LEFT, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(0, szsecond, PA_LEFT, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(2, szMesg, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_SCROLL_LEFT);
  P.addChar('$', degC);
  RTC.control(DS1307_CLOCK_HALT, DS1307_OFF);
  RTC.control(DS1307_12H, DS1307_OFF);
  getTime(szTime);
}

void loop(void)
{
  P.setIntensity(contrast);
  NewStateOfbuttonA = digitalRead(3);
  NewStateOfbuttonB = digitalRead(4);
  NewStateOfbuttonC = digitalRead(5);
  buttonAisPressed();
  buttonBisPressed();
  buttonCisPressed();


      if (buttonA) {
   if (Mode == 0 ) {
    buttonA = false;
    contrast++;
    if (contrast >= 51 ) {
    contrast = 50;
  } 
    }   
  else if (Mode == 1 ) {
    buttonA = false;
      Mode = 0;
  } 
  else if (Mode == 2 ) {
    buttonA = false;
    RTC.h++;
    if (RTC.h >= 24 ) {
    RTC.h = 0;
  }
    RTC.writeTime();
  }
  else if (Mode == 3 ) {
    buttonA = false;
    RTC.m++;
    if (RTC.m >= 60 ) {
    RTC.m = 0;
  }
    RTC.writeTime();
  }
  else if (Mode == 4 ) {
    buttonA = false;
      RTC.s = 0;
    RTC.writeTime();
  }
  else if (Mode == 5 ) {
    buttonA = false;
    RTC.dow++;
    if (RTC.dow >= 8 ) {
    RTC.dow = 1;
  } 
    RTC.writeTime();
    P.displayReset(2);
  }
  else if (Mode == 6 ) {
    buttonA = false;
    RTC.dd++;
    if (RTC.dd >= 32 ) {
    RTC.dd = 1;
  }
    RTC.writeTime();
  }    
  else if (Mode == 7 ) {
    buttonA = false;
    RTC.mm++;
    if (RTC.mm >= 13 ) {
    RTC.mm = 1;
  }
    RTC.writeTime();
  }      
  else if (Mode == 8 ) {
    buttonA = false;
    RTC.yyyy++;
    if (RTC.yyyy >= 2035 ) {
    RTC.yyyy = 2015;
  }  
    RTC.writeTime();
       }
  }

   else if (buttonB) {
    buttonB = false;
      Mode++;
    P.displayReset(2);
      if (Mode >= 9 ) {
      Mode = 0;
  }
    }


    
   if (buttonC) {
   if (Mode == 0 ) {
    buttonC = false;
    contrast--;
    if (contrast <= 0 ) {
    contrast = 0;
  } 
    }   
  else if (Mode == 1 ) {
    buttonC = false;
      Mode = 0;
  } 
  else if (Mode == 2 ) {
    buttonC = false;
    RTC.h--;
    if (RTC.h <= 0 ) {
    RTC.h = 23;
  } 
    RTC.writeTime();
  }
  else if (Mode == 3 ) {
    buttonC = false;
    RTC.m--;
    if (RTC.m <= 0 ) {
    RTC.m = 59;
  } 
    RTC.writeTime();
  }
  else if (Mode == 4 ) {
    buttonC = false;
      RTC.s = 0;
    RTC.writeTime();
  }
  else if (Mode == 5 ) {
    buttonC = false;
    RTC.dow--;
    if (RTC.dow <= 0 ) {
    RTC.dow = 7;
  } 
    RTC.writeTime();
    P.displayReset(2);
  }
  else if (Mode == 6 ) {
    buttonC = false;
    RTC.dd--;
    if (RTC.dd <= 0 ) {
    RTC.dd = 31;
  } 
    RTC.writeTime();
  }    
  else if (Mode == 7 ) {
    buttonC = false;
    RTC.mm--;
    if (RTC.mm <= 0 ) {
    RTC.mm = 12;
  } 
    RTC.writeTime();
  }      
  else if (Mode == 8 ) {
    buttonC = false;
    RTC.yyyy--;
    if (RTC.yyyy <= 2010 ) {
    RTC.yyyy = 2025;
  } 
    RTC.writeTime();
  }  
       }

  if (Mode == 0) 
  { 
  static uint32_t lastTime = 0; // millis() memory
  static bool flasher = false;  // seconds passing flasher
  P.displayAnimate();
  P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
  P.getZoneStatus(1);
  P.getZoneStatus(0);
    if (millis() - lastTime >= 1000)
  {
    lastTime = millis();
    getsecond(szsecond);
    getTime(szTime, flasher);
    flasher = !flasher;
    P.displayReset(1);
    P.displayReset(0);
  } 
  }

  if (Mode == 1) 
  {
  static uint8_t  display = 0;  // current display mode
  P.displayAnimate();
  P.getZoneStatus(2);
if (P.getZoneStatus(2))
  {
    switch (display)
    {
      case 0: // Time
        P.setTextEffect(2, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;
         getTim(szMesg);
        break;

      case 1: // Day
        P.setTextEffect(2, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;
        dow2str(RTC.dow, szMesg, MAX_MESG);
        break;

      case 2:  // Calendar
        P.setTextEffect(2, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;
        getDate(szMesg);
        break;
        
      case 3: // Humidity
        P.setTextEffect(2, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;
         getHumi(szMesg);
          strcat(szMesg, "% RH");
        break;
        
       default: // Temperature deg C
        P.setTextEffect(2, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display = 0;
         getTem(szMesg);
          strcat(szMesg, "$");
        break;
    }

    P.displayReset(2);
  } 
  }


    if (Mode == 2) 
  { 
  static uint32_t lastTime = 0; // millis() memory
  static bool flasher = false;  // seconds passing flasher
  P.displayAnimate();
  P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
  P.getZoneStatus(1);
  P.getZoneStatus(0);
    if (millis() - lastTime >= 200)
  {
    lastTime = millis();
    getsecond(szsecond);
    gethh(szTime, flasher);
    flasher = !flasher;
    P.displayReset(1);
    P.displayReset(0);
  } 
  }



    if (Mode == 3) 
  { 
  static uint32_t lastTime = 0; // millis() memory
  static bool flasher = false;  // seconds passing flasher
  P.displayAnimate();
  P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
  P.getZoneStatus(1);
  P.getZoneStatus(0);
    if (millis() - lastTime >= 200)
  {
    lastTime = millis();
    getsecond(szsecond);
    getmin(szTime, flasher);
    flasher = !flasher;
    P.displayReset(1);
    P.displayReset(0);
  } 
  }


    if (Mode == 4) 
  { 
  static uint32_t lastTime = 0; // millis() memory
  static bool flasher = false;  // seconds passing flasher
  P.displayAnimate();
  P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
  P.getZoneStatus(1);
  P.getZoneStatus(0);
    if (millis() - lastTime >= 200)
  {
    lastTime = millis();
    getTim(szTime);
    getsecond(szsecond);
    P.displayReset(1);
    P.displayReset(0);
  } 
  }

 


 

    if (Mode == 5) 
  { 
  static uint8_t  display = 0;  // current display mode
  P.displayAnimate();
  P.getZoneStatus(2);
        P.setTextEffect(2, PA_PRINT, PA_SCROLL_LEFT);
        dow2str(RTC.dow, szMesg, MAX_MESG);

    P.displayReset(2);
  }


 

    if (Mode == 6) 
  { 
  P.displayAnimate();
  P.getZoneStatus(2);
    P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
        getdyy(szMesg);
    P.displayReset(2);
  }

    if (Mode == 7) 
  { 
  P.displayAnimate();
  P.getZoneStatus(2);
    P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
        getmon(szMesg);
    P.displayReset(2);
  }


 

    if (Mode == 8) 
  { 
  P.displayAnimate();
  P.getZoneStatus(2);
    P.setTextEffect(2, PA_PRINT, PA_NO_EFFECT);
        getyyyy(szMesg);
    P.displayReset(2);
  }
 
 
}
  void buttonAisPressed()
  {
    if (NewStateOfbuttonA != StateOfbuttonA) 
  {
    if (NewStateOfbuttonA == 0) 
    {
      buttonA=true;
    }
    delay(50);
  }
   StateOfbuttonA = NewStateOfbuttonA;
  }

void buttonBisPressed()
{
  if (NewStateOfbuttonB != StateOfbuttonB) 
  {
    if (NewStateOfbuttonB == 0) {
      buttonB=true;
    }
    delay(50);
  }
   StateOfbuttonB = NewStateOfbuttonB;
}

void buttonCisPressed()
{
   if (NewStateOfbuttonC != StateOfbuttonC) 
  {
    if (NewStateOfbuttonC == 0) {
      buttonC=true;
    }
    delay(50);
  }
   StateOfbuttonC = NewStateOfbuttonC;
}
