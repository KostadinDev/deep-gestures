// Arduino_LSM9DS1 - Version: Latest 
#include <Arduino_LSM9DS1.h>

/*

*/
const int SWITCH_PIN = 2;
const char SHAPE = 'O';
int samplerate;
int delayTime = 10;

float x, y, z, xrot, yrot, zrot;
int numData;
int numMotions = 0;
unsigned long t = 0;


void setup() {
  pinMode(SWITCH_PIN, INPUT);
  Serial.begin(9600);
  if(!IMU.begin()){
    Serial.print("IMU failed to initialize");
  }

  Serial.print("Shape,");
  Serial.println(SHAPE);
}

void loop() {
  if(digitalRead(SWITCH_PIN) == HIGH && IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ){
    t = millis();
    numData = 0;
    
    Serial.print("BEGIN,");
    Serial.print(t);
    Serial.print(",");
    Serial.println(++numMotions);
    while(digitalRead(SWITCH_PIN) == HIGH){
      IMU.readAcceleration(x,y,z);
      IMU.readGyroscope(xrot,yrot,zrot);

      Serial.print(millis() - t);
      Serial.print(",");
      Serial.print(x,5);
      Serial.print(",");
      Serial.print(y,5);
      Serial.print(",");
      Serial.print(z,5);
      Serial.print(",");
      Serial.print(xrot,5);
      Serial.print(",");
      Serial.print(yrot,5);
      Serial.print(",");
      Serial.println(zrot,5);
      numData++;
      delay(delayTime);
    }
    Serial.print("EXIT,");
    Serial.println(numData);
  }
  delay(delayTime);
    
}
