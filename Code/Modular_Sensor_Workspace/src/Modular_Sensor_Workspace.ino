/*
 * Project Modular_Sensor_Workspace
 * Description: workspace intended to refine sensor modules to integrate in capstone project
 * Author: Mauricio Villa
 * Date: 12 - August - 2020
 */

/* CHANGE GITHUB BRANCH TO DEVELOPMENT INSTEAD OF MASTER
*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <Grove_Air_Quality_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_MQTT.h>
  #include "Adafruit_MQTT/Adafruit_MQTT.h"
  #include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
  #include "Adafruit_MQTT/Adafruit_MQTT.h"

  #define AIO_SERVER "io.adafruit.com"
  #define AIO_SERVERPORT 1833
  #define AIO_USERNAME "mauriciov99" 
  #define AIO_KEY "thisisarandomstringforakey" //replace
/*      PUT AIO KEYS IN IGNORE FILE       */

  #define OLED_RESET A0

/*      for SD logging        */

unsigned long logTime;
bool logStart; 
const int chipSelect = SS;
int i;
SdFat sd;
SdFile file;
#define FILE_BASE_NAME "SHData"
char fileName[13] = FILE_BASE_NAME "00.csv";
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) -1;
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

/*    for AirQualitySensor use    */
int quality;
int AQvalue;

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

  bme.begin(0x76);
  senseAQ.init();
/*
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
BMEreads();
Serial.printf("temp: %0.2f alt: %0.2f M press: %0.2f hum: %0.2f \n",temp,alt,press,hum);
delay(5000);

}

/*      function for starting up the connection to MQTT       */
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
    // do something for very high pollution
  }
  else if(quality == AirQualitySensor::HIGH_POLLUTION){
    // do something for high pollution
  }
  else if(quality == AirQualitySensor::LOW_POLLUTION){
    // do something for low pollution
  }
  else if(quality == AirQualitySensor::FRESH_AIR){
    // do something for fresh air
  }
}
/* for the above function, maybe tie leds or neopixels to the module
tie a neopixel into the actual transducer frame, only blink red in users peripherals
if dangerous value. if "danger acknowledged" (make dangerAcknowleged a boolean) 
then just make a dim red emit in users peripherals. 
*/

/*        function for the dust sensor        */
/*
void dustSensor(){
  dustDuration = pulseIn( ,LOW); // put in the pin for dustsensor here. 
  lowPulseOccupance = lowPulseOccupancy+dustDuration;

  if((millis()-dustStartTime)>     ){ // put in a value assigned to time. in plant water it was "replace this time"

  } 

}
*/

/*      function for writing to an SD card        */
/* may need to tinker with this. seems to repeat logging data over and over (that was the original intended purpose)
can probably just init like if(dangerTooHigh){ log2SD} */

void log2SD(){
  unsigned long startTime;
  logStart = true;
  if(logStart == true){
    Serial.printf("starting data logging\n");
    while(sd.exists(fileName)){
      if(fileName[BASE_NAME_SIZE +1] != '9'){
        fileName[BASE_NAME_SIZE +1]++;
      }
      else if (fileName[BASE_NAME_SIZE] != '9'){
        fileName[BASE_NAME_SIZE +1] = '0';
        fileName[BASE_NAME_SIZE]++;
      }
      else {
        Serial.println("cant create file name");
        while(1);
      }
    }
    Serial.printf("logging to : %s \n",fileName);
    startTime = micros();
  }
  while(logStart==true){
    for(i=0;i<100;i++){
      logTime = micros() - startTime;
      Serial.print(".");
      // put in functon for data. called logData2 in refrence code
      if(!file.sync() || file.getWriteError()){
        Serial.printf("write error");
      }
      // delay? 
    }
    logStart = false;
    if(logStart == false){
      file.close();
      Serial.printf("done\n");
      delay(2000);
      Serial.printf("Ready for next data log \n");
    }
  }
}

void BMEreads(){
  temp = (bme.readTemperature()* 9/5)+32; // converted to fahrenheit becasue 'merica
  hum = bme.readHumidity();
  press = (bme.readPressure() / 100.0F);
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
}