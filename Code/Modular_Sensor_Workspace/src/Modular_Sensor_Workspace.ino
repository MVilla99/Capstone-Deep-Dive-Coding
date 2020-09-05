/*
 * Project Modular_Sensor_Workspace
 * Description: workspace intended to refine sensor modules to integrate in capstone project
 * Author: Mauricio Villa
 * Date: 12 - August - 2020
 */
SYSTEM_MODE(SEMI_AUTOMATIC)
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
unsigned long last;

/*    for Air Quality Sensor      */
AirQualitySensor senseAQ(A3);

/*    for NeoPixels       */
  #define PIXEL_PIN A1
  #define PIXEL_COUNT 2
  #define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixel(PIXEL_COUNT,PIXEL_PIN,PIXEL_TYPE);
int luminoscity;
bool buttonState = false;
bool pixelState = false;
int Bpin = D2;

/*    for AirQualitySensor use    */
int quality;
int AQvalue;
int qualityValue;

/*    for BME use     */
Adafruit_BME280 bme;
  #define SEALEVELPRESSURE_HPA (1013.25)
float temp;
float press;
float hum;
float alt;

/*    for MQ-9 use      */ 
const int MQaddress = 0x50;
unsigned int MQData[2];
int MQrawADC;
float COppm = 0.0;
int MQval;

/*    for syncing particle clock      */
char currentDateTime[25], currentTime[9];

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial1.begin(9600); // for using with the DFRobot player
  delay(100); // waiting for serial monitor to initialize 
  Wire.begin();

  last = 0; // for MQTT subscription timer. 

  pixel.begin();
  pixel.clear();
  pixel.show();

  bme.begin(0x76);
  senseAQ.init();

  pinMode(Bpin, INPUT_PULLDOWN);

  if(!SD.begin(SD_CS_PIN)){
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("SDlog init");
  /*  if(!myDFP.begin(Serial1)){
    Serial.println("DFPlayer init failed");
    while(true);
  } */
  Serial.println("DFPlayer init");

/*                          commented this chunk out while i tested the bme and other sensors.
  mqtt.subscribe(&subData);
  */
 Serial.println("Initialization finished");
 attachInterrupt(Bpin, enableButton, RISING);
}

void loop() {
//MQTT_connect(); // the name for the function has changed

  HighQualityLED();
 // BMERead();
  //myDFP.playMp3Folder(4); //switch all DFP functions to playMP3Folder
  //delay(10000);
 //MQ9Read();
 //Serial.println(MQval);
 //delay(500);
}

void LEDBrightness(){ // function for using the photoresistor to adjust the brightness of the NeoPixels to be relative to the lighting of the enviornment.
  int pVal;
  int pPin = A2;
  pVal = analogRead(pPin);  // dont have pinmode in setup but works anyways?
  luminoscity = map(pVal, 40, 3000,10,255);
}  
void HighQualityLED(){
  if(pixelState){
    pixel.clear();
    pixel.setPixelColor(0,green);
    pixel.setPixelColor(1,green);
    pixel.setBrightness(100);  // replace with luminoscity  
    pixel.show();
  }
  else if(!pixelState){
    pixel.clear();
    pixel.show();
  }
}
void MidQualityLED(){
  if(pixelState){
    pixel.clear();
    pixel.setPixelColor(0, yellow);
    pixel.setPixelColor(1, yellow);
    pixel.setBrightness(luminoscity);
    pixel.show();
  }
  else if(!pixelState){
    pixel.clear();
    pixel.show();
  }
}
void LowQualityLED(){
  if(pixelState){
    pixel.clear();
    pixel.setPixelColor(0, orange);
    pixel.setPixelColor(1, orange);
    pixel.setBrightness(luminoscity);
    pixel.show();
  }
  else if(!pixelState){
    pixel.clear();
    pixel.show();
  }
}
void DangerLED(){
  if(pixelState){
    pixel.clear();
    pixel.setPixelColor(0, red);
    pixel.setPixelColor(1, red);
    pixel.setBrightness(luminoscity);
    pixel.show();
  }
  else if(!pixelState){
    pixel.clear();
    pixel.show();
  }
}

void MQTTPublish(){ // function for publishing sensor values to adafruit.io
  if((millis()-last)>15000){ // set to publish every 15 seconds, can be adjusted to publish however often you want.
    if(mqtt.Update()){
      PubBME.publish(temp); // used to publish temperature, but can also be used to publish all other readable BME values
      PubMQ9.publish(MQval);
      PubAQ.publish(qualityValue);
    }
  last = millis();
  }
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
void AirQualityRead(){
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

/*      function for the MQ-9 sensor      */
void MQ9Read(){
  Wire.beginTransmission(MQaddress);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(MQaddress ,2, true);
  MQData[0] = Wire.read();
  MQData[1] = Wire.read();
  MQrawADC = ((MQData[0] & 0x0F)*256)+MQData[1];
  COppm = (1000.0/4096.0)*MQrawADC +10.0;
  //Serial.printf("CO: %0.2f ppm \n",COppm);
  MQval = map((int)COppm,0,1200,0,4);
}

/*      function for reading the BME values       */
void BMERead(){
  temp = (bme.readTemperature()* 9/5)+32;
  hum = bme.readHumidity();
  press = (bme.readPressure() / 100.0F);
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
}

void WarningMessage(){ // this function reads the sensory data and outputs a meassage accordingly 
  file = SD.open("DataLog.csv", FILE_WRITE);
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
    MidQualityLED();
    if(file){
      Serial.printf("Air Quality warning. AQ read: %i \n", qualityValue); 
      file.printf("Air Quality Read: %i \n", qualityValue);
      file.print(currentDateTime);
      file.close();
    }
    if(!file){ // if theres an error with the file, log it
      Serial.println("AQ write error");
    }
  }
  else if(qualityValue<=2&& MQval>=3){ //statement for high MQ-9 pollution
    LowQualityLED();
    if(file){
      Serial.printf("MQ-9 warning. MQ-9 read: %i \n", MQval);
      file.printf("MQ-9 read: %i \n", MQval);
      file.print(currentDateTime);
      file.close();
      // myDFP.playFolder(11, ); // switch to the mp3folder function
     // delay( ); // each DFP audio file needs a delay in seconds to let the audio file play
    }
    if(!file){
      Serial.println("MQ-9 write error");
    }
  }
    else if(qualityValue<=2&&MQval<=2){ // function for normal/clean readings
      HighQualityLED();
      if(file){
        Serial.printf("nominal reads. MQ9: %i AQ: %i Temperature: \n", MQval, qualityValue,temp);
        file.println("nominal readings. nothing to record");
        file.print(currentDateTime);
        file.close();
        // myDFP.playFolder(11, );
        // delay( );
      }
      if(!file){
        Serial.println("nominal readings write error.");
      }
    }
  else if(qualityValue>=3 && MQval>=3 && temp>=100){ //statement for high levels of all sensors Ask Brian about the temp with the stuff at the top of this function
    DangerLED();
    if(file){
      Serial.printf("DANGER IMMINANT. MQ-9: %i AQ: %i Temp: %i \n", MQval, qualityValue, temp); 
      file.printf("High Danger. MQ-9: %i AQ: %i Temp %i \n", MQval, qualityValue, temp);
      file.print(currentDateTime);
      file.close();
     // myDFP.playFolder(11, );
     // delay( );
    }
    if(!file){
      Serial.println("High danger write error.");
    }
  }
  if(file){ // if the files open, close it
    file.close();
    }
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

void  enableButton() //ISR for switching the neopixels on or off
{
  pixelState = !pixelState;
}