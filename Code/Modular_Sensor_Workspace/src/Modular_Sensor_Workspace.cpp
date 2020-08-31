/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/mauri/Documents/IoTc2/Capstone-Deep-Dive-Coding/Code/Modular_Sensor_Workspace/src/Modular_Sensor_Workspace.ino"
/*
 * Project Modular_Sensor_Workspace
 * Description: workspace intended to refine sensor modules to integrate in capstone project
 * Author: Mauricio Villa
 * Date: 12 - August - 2020
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <Grove_Air_Quality_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_Sensor.h>
#include <neopixel.h>
#include "colors.h"

#include <Adafruit_MQTT.h>
  #include "Adafruit_MQTT/Adafruit_MQTT.h"
  #include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
  #include "Adafruit_MQTT/Adafruit_MQTT.h"

  void setup();
void loop();
void MQTT_connect();
void airQualitySensor();
void BMEreads();
void SDlog();
void warningMessage();
void highQualityLED();
void midQualityLED();
void lowQualityLED();
void DangerLED();
#line 24 "c:/Users/mauri/Documents/IoTc2/Capstone-Deep-Dive-Coding/Code/Modular_Sensor_Workspace/src/Modular_Sensor_Workspace.ino"
#define AIO_SERVER "io.adafruit.com"
  #define AIO_SERVERPORT 1833
  #define AIO_USERNAME "mauriciov99" 
  #define AIO_KEY "thisisarandomstringforakey" //replace
/*      PUT AIO KEYS IN IGNORE FILE       */

  #define OLED_RESET A0

/*      for SD logging        */
int i;
SdFat SD;
File file;
  #define SD_CS_PIN SS
  #define error(msg) sd.errorHalt(msg)


/*      for subscribing | publishing        */
  TCPClient TheClient;
  Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);
  Adafruit_MQTT_Subscribe subData = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ "); // put feed
  Adafruit_MQTT_Publish pubData = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds "); // put feed

  
Adafruit_BME280 bme; // for bme 
  #define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_SSD1306 display(OLED_RESET); // for oled
AirQualitySensor senseAQ(A2); // put sensor pin in here

/*    for NeoPixels       */
  #define PIXEL_PIN A0// add pin for pixels
  #define PIXEL_COUNT 2// put number of pixels
  #define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixel(PIXEL_COUNT,PIXEL_PIN,PIXEL_TYPE);
int pixNum = 1;

/*    for AirQualitySensor use    */
int quality;
int AQvalue;
int qualityValue; 
/*    for BME use     */
float temp;
float press;
float hum;
float alt;

void setup() {
  Serial.begin(9600);
  delay(100); // waiting for serial monitor to initialize 
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.display();

  pixel.begin();
  pixel.show();

  bme.begin(0x76);
  senseAQ.init();

  if(!SD.begin(SD_CS_PIN)){
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("SD init");
/*                          commented this chunk out while i tested the bme and other sensors.
  mqtt.subscribe(&subData);

  if(!sd.begin(chipSelect,SD_SCK_MHZ(50))){
    sd.initErrorHalt();
  }
  if(BASE_NAME_SIZE > 6){
    Serial.println("FILE_BASE_NAME too long");
    while(1);
  }
  file.printf("timestamp, whatever data"); // printing data header. "timestamp" and "data" are remanents
  */
}


void loop() {
//MQTT_connect(); // still need to impliment the subscribe/publish code.
highQualityLED();
delay(2000);
midQualityLED();
delay(2000);
lowQualityLED();
delay(2000);
}

/*      function for starting up the connection to MQTT. dont forget to do IFTTT       */
void MQTT_connect(){
  int8_t ret;
  if(mqtt.connected()){ // if mqtt is connected, stop
    return;
  }
  Serial.print("connecting to MQTT ...");
  while((ret = mqtt.connect()) != 0){ // if connected, will return a 0
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("MQTT connected?");
}

/*        function for the airquality sensor        */
void airQualitySensor(){
  quality = senseAQ.slope();
  AQvalue = senseAQ.getValue();

  if(quality == AirQualitySensor::FORCE_SIGNAL){
    qualityValue = 4;
  }
  else if(quality == AirQualitySensor::HIGH_POLLUTION){
    qualityValue = 3;
  }
  else if(quality == AirQualitySensor::LOW_POLLUTION){
    qualityValue = 2;
  }
  else if(quality == AirQualitySensor::FRESH_AIR){
    qualityValue = 1;
    // write different neopixel functions for, dim warning, high warning, blink warning
  }
}
/* for the above function, maybe tie leds or neopixels to the module
tie a neopixel into the actual transducer frame, only blink red in users peripherals
if dangerous value. if "danger acknowledged" (make dangerAcknowleged a boolean) 
then just make a dim red emit in users peripherals. 

for the neoPixels, i can write a header file for the colors 
*/

void BMEreads(){
  temp = (bme.readTemperature()* 9/5)+32; // converted to fahrenheit becasue 'merica
  hum = bme.readHumidity();
  press = (bme.readPressure() / 100.0F);
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
}
/*       function below is an exampled of formating the SD logging        */
void SDlog(){
  file = SD.open(" ", FILE_WRITE); // insert name of file. maybe find a way to generate new files?
  // dont forget files arent auto generated from this code.
  if(file){
    Serial.println("writing to file");
    file.println("insert text here");
    file.close();
  }
  else {
    Serial.println("error opening file");
  }
}
 
// replace n with qualityValue
int s; // variable for MQ-9
void warningMessage(){ // this function reads the sensory data and outputs a meassage accordingly 
// assuming that the MQ-9 is coded in a way like the AQ sensor, i have 4 quantitative subroutines 
  file = SD.open(" ", FILE_WRITE); // insert file name. try experimenting with the excel file type
  if(qualityValue>=3 && s<=2){
    // look up syntax for ISR 
    // mp3 file for high pollution
    if(file){ // write the air quality value to the SD, and serial monitor (for testing purposes)
      Serial.printf("Air Quality warning. AQ read: %i \n", qualityValue); 
      file.printf("Air Quality Read: %i \n", qualityValue); // dont forget to write the timestamp to the card/serial monitor 
      file.close();
    }
    if(!file){ // if theres an error with the file, log it
      Serial.println("AQ write error");
      file.println("AQ write error");
      file.close();
    }
  }
  else if(qualityValue<=2&& s>=3){
    // ISR 
    // mp3 file for high MQ-9 reading
    if(file){
      Serial.printf("MQ-9 warning. MQ-9 read: %i \n", s);
      file.printf("MQ-9 read: %i \n", s);
      file.close();
    }
    if(!file){
      Serial.println("MQ-9 write error");
      file.println("MQ-9 write error");
      file.close();
    }
  }
  else if(qualityValue>=3 && s>=3 && temp>=100){
    // ISR 
    // mp3 file for "all sensors above nominal parameters"
    if(file){
      Serial.printf("DANGER IMMINANT. MQ-9: %i AQ: %i Temp: %i \n", s, qualityValue, temp); 
      file.printf("High Danger. MQ-9: %i AQ: %i Temp %i \n", s, qualityValue, temp);
      file.close();
    }
    if(!file){
      Serial.println("High danger write error.");
      file.println("High danger write error.");
      file.close();
    }
    // file.close(): ? do i need this in case none of the functions are enabled. 
  }
  // could also write a statement for "high decibel reading" warning could read as follows; "decibel reading above nominal parameters, ear protection reccomended"
}

// i can maybe make this a switch case statement and embed it i warning message function
void highQualityLED(){
  pixel.clear();
  pixel.setPixelColor(pixNum, green);
  pixel.setBrightness(40);
  pixel.show();
}
void midQualityLED(){
  pixel.clear();
  pixel.setPixelColor(pixNum, yellow);
  pixel.setBrightness(75);
  pixel.show();
}
void lowQualityLED(){
  pixel.clear();
  pixel.setPixelColor(pixNum, orange);
  pixel.setBrightness(100);
  pixel.show();
}
void DangerLED(){
unsigned long pixStart;
unsigned long pixEnd;
  pixStart = millis();
  pixel.clear();
  pixel.setPixelColor(pixNum, red); // forgot to put in the actual pixel number for all the above functions 
  pixel.setBrightness(200);
  pixel.show();
  pixEnd = (millis()-pixStart); // keep working on this one
  if(pixEnd>=1000){
    pixel.setBrightness(0);
    pixel.show();    
  } 
}