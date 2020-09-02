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
#include <DFRobotDFPlayerMini.h>

#include <Adafruit_MQTT.h>
  #include "Adafruit_MQTT/Adafruit_MQTT.h"
  #include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
  #include "Adafruit_MQTT/Adafruit_MQTT.h"

  #define AIO_SERVER "io.adafruit.com"
  #define AIO_SERVERPORT 1833
  #define AIO_USERNAME "mauriciov99" 
  #define AIO_KEY "thisisarandomstringforakey" //replace
/*      PUT AIO KEYS IN IGNORE FILE       */

  #define OLED_RESET A0 // for oled display

/*      for DFRobot mp3 player      */
DFRobotDFPlayerMini myDFP;


/*      for SD logging        */
int i;
SdFat SD;
File file;
  #define SD_CS_PIN SS
  #define error(msg) sd.errorHalt(msg)


/*      for subscribing | publishing        */
  TCPClient TheClient;
  Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);
  Adafruit_MQTT_Subscribe subData = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ "); // put feed if any subscription needed
  Adafruit_MQTT_Publish PubBME = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Smart_Helmet_BME");
  Adafruit_MQTT_Publish PubMQ9 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Smart_Helmet_MQ-9");
  Adafruit_MQTT_Publish PubAQ = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Smart_Helmet_AirQuality");

  
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
int luminoscity;

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
  delay(100);
  Serial1.begin(9600); // for using with the DFRobot player
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

/*    // commented out for testing the mp3 player
  if(!SD.begin(SD_CS_PIN)){
    Serial.println("initialization failed!");
    return;
  }
  */
  Serial.println("SD init");
  if(!myDFP.begin(Serial1)){
    Serial.println("DFPlayer init failed");
    while(true);
  }
  Serial.println("DFPlayer init");
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
//MQTT_connect(); // still need to impliment the subscribe/publish code. also now the name for the function has changed.
  myDFP.playFolder(11, 1);
  Serial.println("playing...");
  delay(5000);  
}

/*      function for starting up the connection to MQTT. dont forget to do IFTTT       */
void MQTTConnect(){
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
void AirQuality(){
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
  }
}

void BMEread(){
  temp = (bme.readTemperature()* 9/5)+32;
  hum = bme.readHumidity();
  press = (bme.readPressure() / 100.0F);
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
}

/*       function below is an exampled of formating the SD logging        */      // FUNCTION DEPRECATED
void SDLog(){
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
 
int s; // variable for MQ-9
void WarningMessage(){ // this function reads the sensory data and outputs a meassage accordingly 
// assuming that the MQ-9 is coded in a way like the AQ sensor, i have 4 quantitative subroutines 
  file = SD.open(" ", FILE_WRITE); // insert file name. try experimenting with the excel file type
  if(qualityValue>=3 && s<=2){
    if(file){ // write the air quality value to the SD, and serial monitor (for testing purposes)
      Serial.printf("Air Quality warning. AQ read: %i \n", qualityValue); 
      file.printf("Air Quality Read: %i \n", qualityValue); // dont forget to write the timestamp to the card/serial monitor. if the particle is going to be connected, then i can use the timeSync stuff
      file.close();
    }
    if(!file){ // if theres an error with the file, log it
      Serial.println("AQ write error");
      file.println("AQ write error");
      file.close();
    }
  }
  else if(qualityValue<=2&& s>=3){
    if(file){
      Serial.printf("MQ-9 warning. MQ-9 read: %i \n", s);
      file.printf("MQ-9 read: %i \n", s);
      file.close();
     // delay( ); // each DFP audio file needs a delay in seconds to let the audio file play
    }
    if(!file){
      Serial.println("MQ-9 write error");
      file.println("MQ-9 write error");
      file.close();
     // myDFP.playFolder(11, );
     // delay( );
    }
  }
  else if(qualityValue>=3 && s>=3 && temp>=100){ 
    if(file){
      Serial.printf("DANGER IMMINANT. MQ-9: %i AQ: %i Temp: %i \n", s, qualityValue, temp); 
      file.printf("High Danger. MQ-9: %i AQ: %i Temp %i \n", s, qualityValue, temp);
      file.close();
     // myDFP.playFolder(11, );
     // delay( );
    }
    if(!file){
      Serial.println("High danger write error.");
      file.println("High danger write error.");
      file.close();
     // myDFP.playFolder(11, ); // this might loop too much and keep delaying/playing. might not need this function of (!file)
     // delay( ); 
    }
    // file.close(): ? do i need this in case none of the functions are enabled. 
  }
}

void LEDBrightness(){ // function for using the photoresistor to adjust the brightness of the NeoPixels to be relative to the lighting of the enviornment.
  int pVal;
  int pPin = A1;
  pVal = analogRead(pPin);
  luminoscity = map(pVal, 40, 3000,10,255);
  // photoresistor fully covered is at 37k
  // with flourescent lights its 22k
  // with flashlight on top of it, its ~20 

}  
void HighQualityLED(){
  pixel.clear();
  //pixel.fill(green, 0, 2); // ported adafruit neopixel headerfile has no member function for fill 
  pixel.setBrightness(luminoscity);
  pixel.show();
}
void MidQualityLED(){
  pixel.clear();
  pixel.setPixelColor(pixNum, yellow);
  pixel.setBrightness(luminoscity);
  pixel.show();
}
void LowQualityLED(){
  pixel.clear();
  pixel.setPixelColor(pixNum, orange);
  pixel.setBrightness(luminoscity);
  pixel.show();
}
void DangerLED(){
  pixel.clear();
  pixel.setPixelColor(pixNum, red); // forgot to put in the actual pixel number for all the above functions 
  pixel.setBrightness(luminoscity);
  pixel.show();
}

void MQTTPublish(){ // function for publishing sensor values to adafruit.io
  unsigned long last;
  if((millis()-last>15000)){ // set to publish every 15 seconds, can be adjusted to publish however often you want.
    if(mqtt.Update()){
      PubBME.publish(temp); // used to publish temperature, but can also be used to publish all other readable BME values
      PubMQ9.publish(s); // put "s" in for now as its the stand-in variable for the MQ-9
      PubAQ.publish(qualityValue);
    }
  last = millis();
  }
}