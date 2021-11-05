#include "tfLiteWrapper.h"
#include <Arduino.h>

/*
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h" // provides operations used by interpreter
#include "tensorflow/lite/micro/micro_error_reporter.h" // outputs debug info
#include "tensorflow/lite/micro/micro_interpreter.h" // code to load and run models
#include "tensorflow/lite/schema/schema_generated.h" // contains schema for TFlite FlatBuffer
#include "tensorflow/lite/version.h"// Provides versioning info
*/

namespace tflWrapper{
    // TfLiteWrapper::
    /**
     * note: for this class, private functions prefixed with tf
     * just deal with tf syntax
     */
    //   tfLiteWrapper();
      TfLiteWrapper::TfLiteWrapper(){
        Serial.println("tflwrapper success");
      }

      void  TfLiteWrapper:: tfSetup(){
        tfSetupLogger();
        tfSetupModel();
        tfSetupInterpreter();
        tfAllocateTensors();
        tfSetupInput();
      }


      /**
       * Sets private variable _modelPtr to the char array pointer specified.
       * Must be caled before tfSetup()
       */
      void TfLiteWrapper::loadModel(unsigned char* modelPtr){
          _modelPtr = modelPtr;
      }

      /**
       *  inputPtr: float array that has the data to be loaded
       */
      void TfLiteWrapper::loadInput(float dataIn[128][3], const unsigned int & inputLength){ // HACK: 3 should be a parameter, so input_length should be dependent on program
          float * in = input-> data.f;

          for(int j = 0; j < inputLength; j++){
            for(int k = 0; k < 3; k++){
              *in = dataIn[j][k];
              in++;
            }
          }

      }

      /**
       * This Model runs the inference
       */
      void TfLiteWrapper::invokeModel(){
        TfLiteStatus invoke_status = interpreter->Invoke();

        Serial.println("Invoked inference");

        if (invoke_status != kTfLiteOk) {
          TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on index: %d\n");
          return;
        } else{
          Serial.println("Error tets passed");
        }
      }

      float * TfLiteWrapper::getOutput(){
        return interpreter->output(0)->data.f;
      }


      /************************************
       *  tflite setup functions
       */

      void TfLiteWrapper::tfSetupLogger(){
        static tflite::MicroErrorReporter micro_error_reporter;
        error_reporter = &micro_error_reporter;
      }

      void TfLiteWrapper::tfSetupModel(){
          model = tflite::GetModel(_modelPtr);
          
          if(model -> version()!= TFLITE_SCHEMA_VERSION){
              while(true)
                error_reporter ->Report("Model version does not match Schema");
          }

      }
      
    //   void tfSetupResolver();
      
      void TfLiteWrapper::tfSetupInterpreter(){
          static tflite::AllOpsResolver resolver;
          static tflite::MicroInterpreter static_interpreter(
              model, resolver, tensor_arena, tensor_arena_size, error_reporter);
          interpreter = &static_interpreter;
      }

      void TfLiteWrapper::tfAllocateTensors(){
        TfLiteStatus allocate_status = interpreter->AllocateTensors();
        if(allocate_status != kTfLiteOk){
            while(true) 
            error_reporter -> Report("AllocateTensors() failed");
        }
      }

      void TfLiteWrapper::tfSetupInput(){
        input = interpreter->input(0);
        if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
          (input->dims->data[1] != 128) ||
          (input->dims->data[2] != 3) ||
          (input->type != kTfLiteFloat32)) {
          TF_LITE_REPORT_ERROR(error_reporter,
                              "Bad input tensor parameters in model");
          return;
        }
      }

      

      
}
