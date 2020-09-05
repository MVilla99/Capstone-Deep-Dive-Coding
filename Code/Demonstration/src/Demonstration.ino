/*
 * Project Demonstration
 * Description:
 * Author:
 * Date:
 */
SYSTEM_MODE(SEMI_AUTOMATIC)
#include <neopixel.h>
#include <DFRobotDFPlayerMini.h>
#include "colors.h"
#include <SdFat.h>
#include <DFRobotDFPlayerMini.h>

  #define PIXEL_PIN A1
  #define PIXEL_COUNT 2
  #define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixel(PIXEL_COUNT,PIXEL_PIN,PIXEL_TYPE);
int luminoscity;
//use enum to make switch case 
enum WarningMessages {
  highQuality = 1,
  midQuality = 2,
  lowQuality = 3,
  DangerQuality = 4,
};

/*      for SD logging        */
int i;
SdFat SD;
File file;
  #define SD_CS_PIN SS
  #define error(msg) sd.errorHalt(msg)

/*      for DFRobot mp3 player      */
DFRobotDFPlayerMini myDFP;

/*    for syncing particle clock      */
char currentDateTime[25], currentTime[9];
int message;

int demoBpin = D3; 
bool demoButtonState = false;
int caseSwitch;
bool run = false;

void setup() {
  Serial.begin(9600);
  delay(100);
  pixel.begin();
  pixel.show();
  pinMode(demoBpin, INPUT_PULLDOWN);
  /*
  if(!SD.begin(SD_CS_PIN)){
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("SDlog init");
  if(!myDFP.begin(Serial1)){
    Serial.println("DFPlayer init failed");
    while(true);
  }
  */
  Serial.println("DFPlayer init");
  Serial.println("Initialization finished");
  //attachInterrupt(demoBpin, enableButton, RISING); // works fine but try momentary switch

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  demoButtonState = digitalRead(demoBpin);
  if(demoButtonState){
    //for(caseSwitch=0;caseSwitch<5;caseSwitch++);
    caseSwitch++;
    if(caseSwitch>4){
     caseSwitch=1;//maybe 1 since first case is enum as 1
     delay(100);
    }
  }
  Serial.println(caseSwitch);
  /*
  SyncTime();
  LEDBrightness();
  file = SD.open("DataLog.csv", FILE_WRITE); // insert file name. try experimenting with the excel file type
  switch(caseSwitch){
      case(highQuality):
      HighQualityLED();
      if(file){
        Serial.printf("nominal reads.");
        file.println("nominal readings. nothing to record");
        file.print(currentDateTime);
        file.close();
        // myDFP.playFolder(11, );
        // delay( );
      }
      if(!file){ // if theres an error with the file, log it
        Serial.println("nominal readings write error.");
      }
      break;
      case(midQuality):
        MidQualityLED();
        if(file){
          Serial.printf("Air Quality warning."); 
          file.printf("Air Quality alert");
          file.print(currentDateTime);
          file.close();
        }
        if(!file){
          Serial.println("AQ alert write error");
        }
      break;
      case(lowQuality):
        LowQualityLED();
        if(file){
          Serial.printf("MQ-9 warning.");
          file.printf("MQ-9 alert");
          file.print(currentDateTime);
          file.close();
          // myDFP.playFolder(11, );
          // delay( ); // each DFP audio file needs a delay in seconds to let the audio file play
        }
        if(!file){
          Serial.println("MQ-9 alert write error");
        }
      break;
      case(DangerQuality):
        DangerLED();
        if(file){
          Serial.printf("DANGER IMMINANT."); 
          file.printf("High Danger.");
          file.print(currentDateTime);
          file.close();
          // myDFP.playFolder(11, );
          // delay( );
        }
        if(!file){
          Serial.println("High danger write error.");
        }
      break;  
  }
  */
}

void LEDBrightness(){ // function for using the photoresistor to adjust the brightness of the NeoPixels to be relative to the lighting of the enviornment.
  int pVal;
  int pPin = A2;
  pVal = analogRead(pPin);
  luminoscity = map(pVal, 40, 3000,10,255);
}
void HighQualityLED(){
  pixel.clear();
  pixel.setPixelColor(0,green);
  pixel.setPixelColor(1,green);
  pixel.setBrightness(luminoscity);
  pixel.show();
}
void MidQualityLED(){
  pixel.clear();
  pixel.setPixelColor(0, yellow);
  pixel.setPixelColor(1, yellow);
  pixel.setBrightness(luminoscity);
  pixel.show();
}
void LowQualityLED(){
  pixel.clear();
  pixel.setPixelColor(0, orange);
  pixel.setPixelColor(1, orange);
  pixel.setBrightness(luminoscity);
  pixel.show();
}
void DangerLED(){
  pixel.clear();
  pixel.setPixelColor(0, red);
  pixel.setPixelColor(1, red);
  pixel.setBrightness(luminoscity);
  pixel.show();
}

void SyncTime(){ // syncing particle clock to cloud clock to get accurate time for timestamps
  String DateTime, TimeOnly;
  Time.zone(-6);
  Particle.syncTime();
  waitUntil(Particle.syncTimeDone);
  DateTime = Time.timeStr();
  TimeOnly = DateTime.substring(11,19);
  DateTime.toCharArray(currentDateTime,25);
  TimeOnly.toCharArray(currentTime,9);
}

void TestAllFunctions(){
  Serial.printf("pixel brightness: %i \n", luminoscity);
  Serial.println("timestamp: ");
  Serial.print(currentDateTime);
}

void enableButton(){
  //demoButtonState = true;
  caseSwitch++;
  if(caseSwitch>4){
    caseSwitch = 1;
  }
}