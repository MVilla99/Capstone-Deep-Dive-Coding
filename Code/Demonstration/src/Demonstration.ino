/*
 * Project Demonstration
 * Description:
 * Author:
 * Date:
 */
#include <neopixel.h>
#include <DFRobotDFPlayerMini.h>
#include "colors.h"

  #define PIXEL_PIN // put pin
  #define PIXEL_COUNT 2
  #define PIXEL_TYPE_WS2812B
Adafruit_NeoPixel pixel(PIXEL_COUNT,PIXEL_PIN,PIXEL_TYPE);
int luminoscity;
//use enum to make switch case 
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
  switch(message){
    
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