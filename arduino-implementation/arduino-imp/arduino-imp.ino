#include <Arduino_LSM9DS1.h>
#include <TensorFlowLite.h>

#include "tensorflow/lite/micro/all_ops_resolver.h" // provides operations used by interpreter
#include "tensorflow/lite/micro/micro_error_reporter.h" // outputs debug info
#include "tensorflow/lite/micro/micro_interpreter.h" // code to load and run models
#include "tensorflow/lite/schema/schema_generated.h" // contains schema for TFlite FlatBuffer
#include "tensorflow/lite/version.h"// Provides versioning info
#include "tensorflow/lite/micro/testing/micro_test.h" // testing


// Our Model
#include "model.h"

// Figure out what's going on
#define DEBUG 0


// Memory allocation
const int tensor_arena_size = 60*1024;
uint8_t tensor_arena[tensor_arena_size];
//
TfLiteTensor* input;
tflite::ErrorReporter* error_reporter = nullptr;

/**
 * Data Collection constants
 */
const unsigned int INPUT_LENGTH = 128;
const int SWITCH_PIN = 2;
unsigned int numData = 0;
unsigned long t = 0;
float xIn [INPUT_LENGTH/2];
float yIn [INPUT_LENGTH/2];
float zIn [INPUT_LENGTH/2];
float tIn [INPUT_LENGTH/2];
float xOut [INPUT_LENGTH/2];
float yOut [INPUT_LENGTH/2];
float zOut [INPUT_LENGTH/2];
float tOut [INPUT_LENGTH/2];
int delayTime = 10;

void setup() {
  // don't miss serial output
  
  #if DEBUG
    while(!Serial);
  #endif
  


  // set up logging
  tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  //set up model
  const tflite::Model* model = ::tflite::GetModel(model_tflite);
  if(model -> version() != TFLITE_SCHEMA_VERSION){
    while(true)
      error_reporter->Report("Model version does not match Schema");
  }

  tflite::AllOpsResolver resolver;

  // instantiate interpreter
  tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size, error_reporter);
 

  // Allocate tensors (interpreter allocate mem from tensor_arena to the model's tensors
  TfLiteStatus allocate_status = interpreter.AllocateTensors();
  if(allocate_status != kTfLiteOk){
    while(true) 
      error_reporter -> Report("AllocateTensors() failed");
  }



  // obtain pointer to model's input tensor
    input = interpreter.input(0);
  if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
      (input->dims->data[1] != 128) ||
      (input->dims->data[2] != 3) ||
      (input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }

  if(!IMU.begin()){
    Serial.print("IMU failed to initialize");
  }



  
}

void loop() {
  
  if(digitalRead(SWITCH_PIN) == HIGH && IMU.accelerationAvailable()){
    Serial.println("in switch loop");
    t = millis();
    numData = 0;
    printArr("t: ", tIn, INPUT_LENGTH);
    Serial.print("Taking data in");
    while(digitalRead(SWITCH_PIN) == HIGH && numData < INPUT_LENGTH/2){
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
      // Prime Tout
    for(int i = 0; i < INPUT_LENGTH; i++){
      tOut[i] = i/(float)INPUT_LENGTH * tIn[numData - 1];
    }

    printArr("x: ", xIn, INPUT_LENGTH);
    printArr("y: ", yIn, INPUT_LENGTH);
    printArr("z: ", zIn, INPUT_LENGTH);
    printArr("t: ", tIn, INPUT_LENGTH);
    printArr("x again: ", xIn, INPUT_LENGTH);
    linInter(xIn, tIn, numData, xOut, tOut, INPUT_LENGTH);
    //linInter(yIn, tIn, numData, yOut, tOut, INPUT_LENGTH);
    //linInter(zIn, tIn, numData, zOut, tOut, INPUT_LENGTH);

    printArr("x: ", xIn, INPUT_LENGTH);
    printArr("y: ", yIn, INPUT_LENGTH);
    printArr("z: ", zIn, INPUT_LENGTH);
    printArr("t: ", tIn, INPUT_LENGTH);
    
    printArr("x Interpolated: ", xOut, INPUT_LENGTH);
    printArr("y Interpolated: ", yOut, INPUT_LENGTH);
    printArr("z Interpolated: ", zOut, INPUT_LENGTH);
    printArr("t new: ", tOut, INPUT_LENGTH);
    Serial.print("finished");
  }
  delay(delayTime);

  Serial.print(" 0");
  Serial.print("IMU status: ");
Serial.println(IMU.accelerationAvailable());
}





/**
 * 
 *  Linear interpolation functions
 *  
 */
void linInter(float fi[], float ti[], unsigned int szeIn, float fo[], float to[], int szeOut){
  for(int i = 0; i < szeOut; i++){
    fo[i] = interPt(fi, ti, szeIn, to[i]);
  }
}

double interPt(float f[], float t[], unsigned int arrSze, float tIn){
  if(tIn < t[0])
    return(f[0]);
  // assume array is sorted
  int i;
  for(i = 0; i < arrSze; i++){
    if(tIn > t[i]) break;
  }
  i--;
  float out = (f[i]*(t[i+1] - tIn) + f[i+1]*(tIn - t[i]))/(t[i+1] - t[i]);

  return(out);
}


void printArr(String label, float arr[], unsigned int arrSize ){
  Serial.println(label);
  for(int i = 0; i < arrSize; i++){
    Serial.print(i);
    Serial.print(": ");
    Serial.println(arr[i],5);
  }

  Serial.println("done\n\n");
}
