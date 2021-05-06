/**
 * Librariees and outside files
 */
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#include "primer.h"
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h" // provides operations used by interpreter
#include "tensorflow/lite/micro/micro_error_reporter.h" // outputs debug info
#include "tensorflow/lite/micro/micro_interpreter.h" // code to load and run models
#include "tensorflow/lite/schema/schema_generated.h" // contains schema for TFlite FlatBuffer
#include "tensorflow/lite/version.h"// Provides versioning info
      //#include "tensorflow/lite/micro/testing/micro_test.h" // testing

// Our Model
#include "model.h"

// Not sure what this does, but I saw it somewhere
#define DEBUG 0


/*
 * Variable initialization and memory Allocation
 */
namespace { // for scope
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;

  constexpr int tensor_arena_size = 15*1024;
  uint8_t tensor_arena[tensor_arena_size];

}
              //TfLiteTensor* input;
              //tflite::ErrorReporter* error_reporter = nullptr;

              // Allocate Memory
              //const int tensor_arena_size = 60*1024;
              //uint8_t tensor_arena[tensor_arena_size];

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

// creating BLE com lines
BLEService gestureService("19B10011-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedIntCharacteristic gestureCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

/*****************************************
 * SETUP
 ****************************************/
void setup() {
  // don't miss serial output
  #if DEBUG
    while(!Serial);
  #endif
  


  // set up logging
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  //set up model
  //const tflite::Model* 
  model = tflite::GetModel(model_tflite);
  if(model -> version() != TFLITE_SCHEMA_VERSION){
    while(true)
      error_reporter->Report("Model version does not match Schema");
  }

  // Set up resolver
  static tflite::AllOpsResolver resolver;

  // instantiate interpreter
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, tensor_arena_size, error_reporter);
  interpreter = &static_interpreter;

  // Allocate tensors (interpreter allocate mem from tensor_arena to the model's tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if(allocate_status != kTfLiteOk){
    while(true) 
      error_reporter -> Report("AllocateTensors() failed");
  }



  // obtain pointer to model's input tensor
    input = interpreter->input(0);
  if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
      (input->dims->data[1] != 128) ||
      (input->dims->data[2] != 3) ||
      (input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }

  // Begin IMU
  if(!IMU.begin()){
    Serial.print("IMU failed to initialize");
  }

  pinMode(O_PIN, OUTPUT);
  pinMode(LR_PIN, OUTPUT);
  pinMode(RL_PIN, OUTPUT);
  pinMode(SPIN_PIN, OUTPUT);
  pinMode(THROW_PIN, OUTPUT);

  //initialize BLE comms();
  BLE.begin();
  BLE.setLocalName("GesturePredictor");
  BLE.setAdvertisedService(gestureService);
  gestureService.addCharacteristic(gestureCharacteristic);
  BLE.addService(gestureService);

  gestureCharacteristic.writeValue(0);
  BLE.advertise();

}

void loop() {
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
    float * in = input->data.f;
    for(int j = 0; j < INPUT_LENGTH; j++){
      for(int k = 0; k < 3; k++){
          *in = TFin[j][k];
          in++;
      }
    }


    Serial.println("Tensor Loaded");

    // Perform inference

    TfLiteStatus invoke_status = interpreter->Invoke();
        Serial.println("Invoked inference");
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on index: %d\n");
    return;
  } else{
    Serial.println("Error tets passed");

    //Output results
    float *gesture_pred = interpreter->output(0)->data.f;
    Serial.println("Output assigned");
    Serial.print("Prediction 1: ");
    Serial.print(gesture_pred[0]);
    Serial.print("    Prediction 2: ");
    Serial.print(gesture_pred[1]);
    Serial.print("    Prediction 3: ");
    Serial.print(gesture_pred[2]);
    Serial.print("    Prediction 4: ");
    Serial.print(gesture_pred[3]);
    Serial.print("    Prediction 5: ");
    Serial.println(gesture_pred[4]);

    //writeBLE(gesture_pred);
    
    Serial.println("1");
    if(gesture_pred[0] > .5){
      analogWrite(O_PIN,gesture_pred[0]*255);
      gestureCharacteristic.writeValue(1);
      //digitalWrite(O_PIN, HIGH);
    }
    Serial.println("2");
    if(gesture_pred[1] > .5){
      analogWrite(LR_PIN,gesture_pred[1]*255);
      gestureCharacteristic.writeValue(2);
      //digitalWrite(LR_PIN, HIGH);
    }
    Serial.println("3");
    if(gesture_pred[2] > .5){
      analogWrite(RL_PIN,gesture_pred[2]*255);
      gestureCharacteristic.writeValue(3);
      //digitalWrite(RL_PIN, HIGH);
    }
    Serial.println("4");
    if(gesture_pred[3] > .5){
      analogWrite(SPIN_PIN,gesture_pred[3]*255);
      gestureCharacteristic.writeValue(4);
      //digitalWrite(SPIN_PIN, HIGH);
    }
    Serial.println("5");
    if(gesture_pred[4] > .5){
      gestureCharacteristic.writeValue(5);
      digitalWrite(THROW_PIN, HIGH);
    }
    Serial.println("6");
    delay(1000);
    Serial.println("7");
    analogWrite(O_PIN,0);
    Serial.println("8");
    analogWrite(LR_PIN,0);
    Serial.println("9");
    analogWrite(RL_PIN,0);
    Serial.println("10");
    analogWrite(SPIN_PIN,0);
    Serial.println("11");
    digitalWrite(THROW_PIN,LOW);
    Serial.println("12");
  }

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
