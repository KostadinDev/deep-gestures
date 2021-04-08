/**
 * Librariees and outside files
 */
#include <Arduino_LSM9DS1.h>

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

  constexpr int tensor_arena_size = 60*1024;
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
const int SWITCH_PIN = 2;
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
  //Microresolver added in later
  /*static tflite::MicroMutableOpResolver<4> micro_op_resolver;  // NOLINT
 
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddMaxPool2D();
  //flatten
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddSoftmax();
  */

  // instantiate interpreter
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, tensor_arena_size, error_reporter);
  interpreter = &static_interpreter;
          //tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size, error_reporter);
         //Static interpretter as compared to normal interpreter
          /*static tflite::MicroInterpreter static_interpreter(
              model, micro_op_resolver, tensor_arena, tensor_arena_size, error_reporter);
            interpreter = &static_interpreter;
          */
  // Allocate tensors (interpreter allocate mem from tensor_arena to the model's tensors
  //TfLiteStatus allocate_status = interpreter.AllocateTensors();
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if(allocate_status != kTfLiteOk){
    while(true) 
      error_reporter -> Report("AllocateTensors() failed");
  }



  // obtain pointer to model's input tensor
    //input = interpreter.input(0);
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



  
}

void loop() {
 
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
    printArr("x: ", xIn, INPUT_LENGTH);
    printArr("y: ", yIn, INPUT_LENGTH);
    printArr("z: ", zIn, INPUT_LENGTH);
    printArr("t: ", tIn, INPUT_LENGTH);
    
    
    printArr("x Interpolated: ", xOut, INPUT_LENGTH);
    printArr("y Interpolated: ", yOut, INPUT_LENGTH);
    printArr("z Interpolated: ", zOut, INPUT_LENGTH);
    printArr("t new: ", tOut, INPUT_LENGTH);

    // combine arrays & load tensor
    hstack(xOut,yOut,zOut,TFin, INPUT_LENGTH);
    float * in = input->data.f;
    for(int j = 0; j < INPUT_LENGTH; j++){
      for(int k = 0; k < 3; k++){
          *in = TFin[j][k];
          in++;
      }
    }
    //input -> data.f = *TFin;
    Serial.println(*input->data.f);
    Serial.println("Tensor Loaded");

    // Perform inference
                //interpreter -> ResetVariableTensors();
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
    Serial.print("          Prediction 2: ");
    Serial.print(gesture_pred[1]);
    Serial.print("          Prediction 3: ");
    Serial.print(gesture_pred[2]);
  }

    Serial.print("finished");
  }
  delay(delayTime);

  //Serial.print(" 0");
  //Serial.print("IMU status: ");
  //Serial.println(IMU.accelerationAvailable());
}





/**
 * 
 *  Linear interpolation functions
 *  
 */
void linInter(float fi[], float ti[], unsigned int szeIn, float fo[], float to[], int szeOut){
  Serial.println("----------------------------");
  Serial.println("----------------------------");
  Serial.println("----------------------------");

  Serial.println("----------------------------");
  Serial.println("----------------------------");
  Serial.println("----------------------------");
  for(int i = 0; i < szeOut; i++){
    fo[i] = interPt(fi, ti, szeIn, to[i]);
  }
}


double interPt(float f[], float t[], unsigned int arrSze, float tIn){
  if(tIn < t[0])
    return(f[0]);
  // assume array is sorted
  int i;
  float prev = 0;
  for(i = 0; i < arrSze; i++){
    if(tIn < t[i] && tIn > prev ) break;
    prev = t[i];
  }
  i--;
  return((f[i]*(t[i+1] - tIn) + f[i+1]*(tIn - t[i]))/(t[i+1] - t[i]));
}


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


void hstack(float x[], float y[], float z[], float output[][3], unsigned int arrLen){

    for (int i = 0; i < arrLen; i++){
      output[i][0] = x[i];
      output[i][1] = y[i];
      output[i][2] = z[i];
    }
}
