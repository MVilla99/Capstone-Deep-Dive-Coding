/*
 * Project Demonstration
 * Description:
 * Author:
 * Date:
 */
#include <neopixel.h>
#include <DFRobotDFPlayerMini.h>
#include "colors.h"
#include <SdFat.h>
#include <DFRobotDFPlayerMini.h>

  #define PIXEL_PIN // put pin
  #define PIXEL_COUNT 2
  #define PIXEL_TYPE_WS2812B
Adafruit_NeoPixel pixel(PIXEL_COUNT,PIXEL_PIN,PIXEL_TYPE);
int luminoscity;
//use enum to make switch case 
enum WarningMessages {
  highQuality = 1,
  midQuality = 2,
  lowQuality = 3,
  DangerQuality = 4;
};

/*      for SD logging        */
int i;
SdFat SD;
File file;
  #define SD_CS_PIN SS
  #define error(msg) sd.errorHalt(msg)

/*      for DFRobot mp3 player      */
DFRobotDFPlayerMini myDFP;

int message;
// nstead of incrimenting with button clicks use encoder. or use on button (click = message++)
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);
  delay(100);
  pixel.begin();
  pixel.show();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  LEDBrightness();
  switch(message){
      case(highQuality):

      break;
      case(midQuality):

      break;
      case(lowQuality):

      break;
      case(DangerQuality):

      break;  
  }
}

//insert the warning message function in here once done

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
  Serial.println("pixel working");
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

void WarningMessage(){ // this function reads the sensory data and outputs a meassage accordingly 
  file = SD.open("DataLog.csv", FILE_WRITE); // insert file name. try experimenting with the excel file type
  static int lastQualityValue;
  static int lastMQval;
  if(qualityValue == lastQualityValue && lastMQval == MQval){
    return;
    }
  else{
    lastQualityValue = qualityValue;
    lastMQval = MQval;
  }
  if(qualityValue>=3 && MQval<=2){ // statement for high air quality pollution
    if(file){
      Serial.printf("Air Quality warning. AQ read: %i \n", qualityValue); 
      file.printf("Air Quality Read: %i \n", qualityValue); // dont forget to write the timestamp to the card/serial monitor. if the particle is going to be connected, then i can use the timeSync stuff
      file.print(currentDateTime);
      file.close();
    }
    if(!file){ // if theres an error with the file, log it
      Serial.println("AQ write error");
      file.println("AQ write error");
      file.print(currentDateTime);
      file.close();
    }
  }
  else if(qualityValue<=2&& MQval>=3){ //statement for high MQ-9 pollution
    if(file){
      Serial.printf("MQ-9 warning. MQ-9 read: %i \n", MQval);
      file.printf("MQ-9 read: %i \n", MQval);
      file.print(currentDateTime);
      file.close();
     // delay( ); // each DFP audio file needs a delay in seconds to let the audio file play
    }
    if(!file){
      Serial.println("MQ-9 write error");
      file.println("MQ-9 write error");
      file.print(currentDateTime);
      file.close();
     // myDFP.playFolder(11, );
     // delay( );
    }
  }
  else if(qualityValue>=3 && MQval>=3 && temp>=100){ //statement for high levels of all sensors
    if(file){
      Serial.printf("DANGER IMMINANT. MQ-9: %i AQ: %i Temp: %i \n", MQval, qualityValue, temp); 
      file.printf("High Danger. MQ-9: %i AQ: %i Temp %i \n", MQval, qualityValue, temp);
      file.print(currentDateTime);
      file.close();
     // myDFP.playFolder(11, );
     // delay( );
     DangerLED();
    }
    if(!file){
      Serial.println("High danger write error.");
      file.println("High danger write error.");
      file.print(currentDateTime);
      file.close();
     // myDFP.playFolder(11, ); // this might loop too much and keep delaying/playing. might not need this function of (!file)
     // delay( ); 
    }
    // file.close(): ? do i need this in case none of the functions are enabled. 
  }
}
