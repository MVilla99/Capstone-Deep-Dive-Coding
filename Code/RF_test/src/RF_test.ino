/*
 * Project RF_test
 * Description: testing the RF transmitter/reciever
 * Author: Mauricio Villa
 * Date: 20 - August - 2020
 */
#include <SPI.h>
#include <RF24.h>
bool radioNumber = 0;
RF24 radio( D3,D2);
byte addresses[][6] = {"1Node". "2Node"};
bool role = 0;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }
  else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  radio.startListening();

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  if(role ==1){
    radio.stopListening();
    unsigned long start_time = micros();
    if(!radio.write(&start_time, sizeof(unsigned long) )){
      Serial.printf("failed\n");
    }
  radio.startListening();
  unsigned long started_waiting_at = micros();
  boolean timeout = false;

  while(!radio.available() ){
    if(micros() - started_waiting_at > 200000){
      timeout = true; 
      break;
    }
  }
  if(timeout){
    Serial.println("failed, response timed out");
  }
  else{
    unsigned long got_time;
    radio.read(&got_time, sizeof(unsigned long) );
    unsigned long end_time = micros();
    Serial.println("sent");
    Serial.print(start_time);
    Serial.println("got response");
    Serial.print(got_time);
    Serial.println("round-trip-delay");
    Serial.print(end_time-start_time);
    Serial.println("microseconds");
  }
  delay(1000);
  }
  if(role == 0){
    unsigned long got_time;
    if(radio.available()){
      while(radio.available()){
        radio.read(&got_time,sizeof(unsigned long));
      }
    radio.stopListening();
    radio.write(&got_time, sizeof(unsigned long));
    radio.startListening();
    Serial.println("sent response");
    Serial.print(got_time);
    }
  }
  if(Serial.available()){
    char c = toupper(Serial.read());
    if( c == 'T' && role == 0){
      Serial.println(" changing to transmit role -- press 'r' to switch back");
      role = 1;
    }
    else if( c == 'R' && role = 1){
      Serial.println("changing to receive role -- press 't' to switch back");
      role = 0; 
      radio.startListening();
    }
  }
}