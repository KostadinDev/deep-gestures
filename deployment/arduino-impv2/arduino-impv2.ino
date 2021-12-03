/**
 * Librariees and outside files
 */
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

// tfLite
#include "tfLiteWrapper.h"
#include "primer.h"
#include "output.h"
/*
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h" // provides operations used by interpreter
#include "tensorflow/lite/micro/micro_error_reporter.h" // outputs debug info
#include "tensorflow/lite/micro/micro_interpreter.h" // code to load and run models
#include "tensorflow/lite/schema/schema_generated.h" // contains schema for TFlite FlatBuffer
#include "tensorflow/lite/version.h"// Provides versioning info
      //#include "tensorflow/lite/micro/testing/micro_test.h" // testing
      */

// Our Model
#include "model.h"

// Not sure what this does, but I saw it somewhere
#define DEBUG 0


/**
 * Data Collection constants
 */
unsigned int numData = 0;
const unsigned int INPUT_LENGTH = 128;
const unsigned int SWITCH_PIN = 2;
const unsigned int O_PIN = 3;
const unsigned int LR_PIN = 4;
const unsigned int RL_PIN = 5;
const unsigned int SPIN_PIN = 6;
const unsigned int THROW_PIN = 7;
unsigned long t = 0;
float xIn [INPUT_LENGTH]; //Takes from IMU
float yIn [INPUT_LENGTH];
float zIn [INPUT_LENGTH];
float tIn [INPUT_LENGTH];
float xOut [INPUT_LENGTH]; // Interpolated 
float yOut [INPUT_LENGTH];
float zOut [INPUT_LENGTH];
float tOut [INPUT_LENGTH];

float TFin [INPUT_LENGTH][3];
int delayTime = 10;

int i;

tflWrapper::TfLiteWrapper tfl;

/*****************************************
 * SETUP
 ****************************************/
void setup() {
  Serial.begin(9600);
  // don't miss serial output
  #if DEBUG
    while(!Serial);
  #endif
  


  tfl.loadModel(model_tflite);
  tfl.tfSetup();


  // Begin IMU
  if(!IMU.begin()){
    Serial.print("IMU failed to initialize");
  }

  pinMode(O_PIN, OUTPUT);
  pinMode(LR_PIN, OUTPUT);
  pinMode(RL_PIN, OUTPUT);
  pinMode(SPIN_PIN, OUTPUT);
  pinMode(THROW_PIN, OUTPUT);


  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop() {
  Serial.println("main loop");
  BLE.poll();
  if(digitalRead(SWITCH_PIN) == HIGH && IMU.accelerationAvailable()){
    Serial.println("in switch loop");
    t = millis();
    numData = 0;
    while(digitalRead(SWITCH_PIN) == HIGH && numData < INPUT_LENGTH){
      //Data collection
      IMU.readAcceleration(xIn[numData],yIn[numData],zIn[numData]);

      Serial.print(" x: ");
      Serial.print(xIn[numData]);
      Serial.print(" y: ");
      Serial.print(yIn[numData]);
      Serial.print(" z: ");
      Serial.println(zIn[numData]);
      
      tIn[numData] = (float) (millis()-t);

      numData++;
      delay(delayTime);
      
    }
      // Create evenly spaced time array
    for(int i = 0; i < INPUT_LENGTH; i++){
      tOut[i] = i/(float)INPUT_LENGTH * tIn[numData - 1];
    }

    //linear interpolation
    linInter(xIn, tIn, numData, xOut, tOut, INPUT_LENGTH);
    linInter(yIn, tIn, numData, yOut, tOut, INPUT_LENGTH);
    linInter(zIn, tIn, numData, zOut, tOut, INPUT_LENGTH);

    //vector printing for visualization
    //printArr("x: ", xIn, INPUT_LENGTH);
    //printArr("y: ", yIn, INPUT_LENGTH);
    //printArr("z: ", zIn, INPUT_LENGTH);
    //printArr("t: ", tIn, INPUT_LENGTH);
    
    
    //printArr("x Interpolated: ", xOut, INPUT_LENGTH);
    //printArr("y Interpolated: ", yOut, INPUT_LENGTH);
    //printArr("z Interpolated: ", zOut, INPUT_LENGTH);
    //printArr("t new: ", tOut, INPUT_LENGTH);

    // combine arrays & load tensor
    hstack(xOut,yOut,zOut,TFin, INPUT_LENGTH);
    // ***********************************************************
    tfl.loadInput(TFin, INPUT_LENGTH);



    Serial.println("Tensor Loaded");

    // Perform inference

    // run model
    tfl.invokeModel();

    // output 
    float *gesturePred = tfl.getOutput();
    int pins[5] = {O_PIN, LR_PIN, RL_PIN, SPIN_PIN, THROW_PIN};
    handleOutput(gesturePred, pins, 5);
    Serial.println("finished");
  
  }
  delay(delayTime);
}





/**
 * 
 *  Linear interpolation functions
 *  
 */





void printArr(String label, float arr[], unsigned int arrSize ){
  Serial.println(label);
  
  Serial.print("[ ");
  for(int i = 0; i < arrSize; i++){


    Serial.print(arr[i],5);
    if(i != arrSize - 1) Serial.print(", ");

  }
  Serial.println("]");
  /*
  for(int i = 0; i < arrSize/2; i++){
    Serial.print(i);
    Serial.print(": ");
    Serial.print(arr[i],5);
    Serial.print("        ");
    Serial.print(i + arrSize/2);
    Serial.print(": ");
    Serial.println(arr[i+arrSize/2],5);
  }
*/
  Serial.println("done\n\n");
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  if(x==1){digitalWrite(D13,HIGH);}
  else{digitalWrite(13,LOW);}


  Serial.println(x);         // print the integer
}

void requestEvent()
{
  Serial.println("requestEvent");
  //Wire.write(
  for(int i = 0; i < INPUT_LENGTH; i++){
    Wire.write(TFin[i][0]);
  }
}
