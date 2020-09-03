/*
 * Project MQ9_calibration
 * Description:
 * Author:
 * Date:
 */

int x;
int LED = D7;
int mq = A0;
int i;
// R0 = 9.4 Average
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
Serial.begin(9600);
pinMode(LED, OUTPUT);
pinMode(mq, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
/*
void loop() {
  // The core of your code will likely live here.
  float sensor_volt;
  float RS_air;
  float R0;
  float sensorValue;
    for(x =0; x<100; x++){
      sensorValue = sensorValue + analogRead(A0);
    }
    sensorValue = sensorValue/100.0;

    sensor_volt = (sensorValue/4096)*3.3;
    RS_air = (3.3 - sensor_volt)/sensor_volt;
    R0 = RS_air/9.9;
    Serial.printf("sensor_volt = %0.2f V \n",sensor_volt);
    Serial.printf("R0 = %0.2f \n",R0);
    delay(1000);
}
*/

/*
//    for use when replacing R0 
void loop(){
  int alarm = 0;
  float sensor_volt;
  float RS_gas;
  float ratio;
  float PPM;
  float sensorValue;
  float R0 = 9.4; // whatever val achieved
  sensorValue = analogRead(A0);
  sensor_volt = (sensorValue / 4096.0)* 5.0;
  RS_gas = (5.0 - sensor_volt) / sensor_volt;
  ratio = RS_gas / R0;
  PPM = (1000.0/4096.0)*ratio+10.0;
  Serial.printf("sensor_volt = %0.2f \n", sensor_volt);
  Serial.printf("RS_ratio = %0.2f \n", RS_gas);
  Serial.printf("rs/R0 = %0.2f \n", ratio);
  Serial.println(PPM);
  alarm = digitalRead(D0);
  if(alarm == 1){
    digitalWrite(LED, HIGH);
  }
  else if (alarm == 0){
    digitalWrite(LED, LOW);
  }
  delay(1000);
}
*/

void loop(){
  int read;
  int readAverage;
  float measuredVolts;
  float measuredResistance;
  float R0;
  read = 0;
  for(x=0;x<100;x++){
    read += analogRead(mq);
    delay(20);
  }
  readAverage = read/100;
  measuredVolts = (3.3/4096)*readAverage;
  measuredResistance = ((5.0/measuredVolts)-1)*1000;
  R0 = ((5.0/measuredVolts)-1.0)*(1000.0/measuredResistance);
  Serial.printf("readAverage: %i measured volts: %0.2f measured resistance: %0.2f \n", readAverage,measuredVolts,measuredResistance);
  Serial.printf("R0: %0.2f \n",R0);
}