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

const unsigned int INPUT_LENGTH = 128;

// Memory allocation
const int tensor_arena_size = 60*1024;
uint8_t tensor_arena[tensor_arena_size];
//
TfLiteTensor* input;
tflite::ErrorReporter* error_reporter = nullptr;

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
    error_reporter->Report("Model version does not match Schema");
    while(1);
  }

  tflite::AllOpsResolver resolver;

  // instantiate interpreter
  tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size, error_reporter);
 

  // Allocate tensors (interpreter allocate mem from tensor_arena to the model's tensors
  TfLiteStatus allocate_status = interpreter.AllocateTensors();
  if(allocate_status != kTfLiteOk){
    while(true) {
      error_reporter -> Report("AllocateTensors() failed");
    }
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
  
}

void loop() {
  Serial.print("Test Output\n");
}





/**
 * 
 *  Linear interpolation functions
 *  
 */
void linInter(double fi[], double ti[], unsigned int szeIn, double fo[], double to[], int szeOut){
  for(int i = 0; i < szeOut; i++){
    fo[i] = interPt(fi, ti, szeIn, to[i]);
  }
}

double interPt(double f[], double t[], unsigned int arrSze, double tIn){
  // assume array is sorted
  int i;
  for(i = 0; i < arrSze; i++){
    if(tIn > t[i]) break;
  }
  double out = (f[i]*(t[i+1] - tIn) + f[i+1]*(tIn - t[i]))/(t[i+1] - t[i]);

  return(out);
}
