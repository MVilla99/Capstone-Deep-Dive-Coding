/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/mauri/Documents/IoTc2/Capstone-Deep-Dive-Coding/Code/microphone_DB_calibration/src/microphone_DB_calibration.ino"
/*
 * Project microphone_DB_calibration
 * Description:
 * Author:
 * Date:
 */

void setup();
void loop();
#line 8 "c:/Users/mauri/Documents/IoTc2/Capstone-Deep-Dive-Coding/Code/microphone_DB_calibration/src/microphone_DB_calibration.ino"
int Mpin =A1;
int dB;
int adc;
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(Mpin, INPUT);
  pinMode(D7, OUTPUT);
  Serial.begin(9600);

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  adc = analogRead(Mpin);
  Serial.println(adc);

}
/*
void regressionCalc(){
  adc = analogRead(Mpin);
  dB = (adc + )/  ; // converting ADC val to dB using regression val from excel
  if(dB>60){ // blinking surface mount led on particle if dB is above 60
    digitalWrite(D7, HIGH);
    delay(100);
    digitalWrite(D7, LOW);
    delay(100);
  }
}
*/