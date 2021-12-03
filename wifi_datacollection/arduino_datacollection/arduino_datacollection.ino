#include <Arduino_LSM9DS1.h>

const unsigned int SWITCH_PIN = 2;
const int INPUT_LENGTH = 128;
float xIn [INPUT_LENGTH]; //Takes from IMU
float yIn [INPUT_LENGTH];
float zIn [INPUT_LENGTH];
int numData = 0;
int delayTime = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);

  
  if(!IMU.begin()){
    Serial.print("IMU failed to initialize");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial1.print("11111111122222222223333333333444444444455555555556666666667777777777888888888999999999900000000012345678901111111111222222222233333333334444444444");
  //Serial1.print("|");
  //Serial.println("starting");

  if(digitalRead(SWITCH_PIN) == HIGH && IMU.accelerationAvailable()){
    //Serial.println("in switch loop");
    numData = 0;
    while(digitalRead(SWITCH_PIN) == HIGH && numData < INPUT_LENGTH){
      //Data collection
      IMU.readAcceleration(xIn[numData],yIn[numData],zIn[numData]);

      //Serial.print(" x: ");
      Serial.println(xIn[numData],4);
      //Serial1.print(xIn[numData],4);
      
      //Serial.print(" y: ");
      //Serial.print(yIn[numData],4);
      //Serial.print(" z: ");
      //Serial.println(zIn[numData],4);
      

      numData++;
      delay(delayTime);
      
    }

  
  for(int i = 0; i < INPUT_LENGTH; i++){
    Serial1.print(xIn[i],4);
    Serial1.println();
    delay(10);
    
    //if(i % 20 == 0)
      //delay(100);
    
  }
  }
  delay(500);

}

void sendFloat(float f){
  byte * b = (byte *) &f;
  Serial1.print("f:");
  Serial1.write(b[0]);
  Serial1.write(b[1]);
  Serial1.write(b[2]);
  Serial1.write(b[3]);
  Serial1.print(68); //Send nonsense.. Else serial drops offline??
  Serial1.flush();
  return;
}
