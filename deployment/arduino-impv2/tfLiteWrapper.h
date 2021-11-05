#ifndef TF_LITE_WRAPPER_H
#define TF_LITE_WRAPPER_H

#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h" // provides operations used by interpreter
#include "tensorflow/lite/micro/micro_error_reporter.h" // outputs debug info
#include "tensorflow/lite/micro/micro_interpreter.h" // code to load and run models
#include "tensorflow/lite/schema/schema_generated.h" // contains schema for TFlite FlatBuffer
#include "tensorflow/lite/version.h"// Provides versioning info

namespace tflWrapper{
  class TfLiteWrapper{
    /**
     * note: for this class, private functions prefixed with tf
     * just deal with tf syntax
     */
    public:
      //tflite::ErrorReporter* error_reporter = nullptr;
      const tflite::Model* model = nullptr;
      tflite::MicroInterpreter* interpreter = nullptr;
      TfLiteTensor* input = nullptr;
      TfLiteTensor* output = nullptr;

      static constexpr int tensor_arena_size = 15*1024;
      uint8_t tensor_arena[tensor_arena_size];

      tflite::ErrorReporter* error_reporter = nullptr;
      //static tflite::AllOpsResolver resolver;



      void tfSetup();

      /**
       * These functions are going to be used when more models with more
       * diverse input are to be implemented
       */
      TfLiteTensor* setupInput(); // TODO: unimplemented, but needed later
      void loadInput(float* dataPtr); // TODO: unimplemented general purpose input



      /**
       * These should be called!
       */
      TfLiteWrapper();

      void loadModel(unsigned char* modelPtr);

      //TODO: make work for multiple types and multiple sizes
      void loadInput(float dataIn[128][3], const unsigned int & inputLength); // HACK: 3 should be a parameter, so input_length should be dependent on program
      void invokeModel(); // TODO: make into boolean
      float * getOutput(); //TODO: make work with different types


    private:
      unsigned char * _modelPtr;

      // setup tfLite
      void tfSetupLogger();
      void tfSetupModel();
        // void tfSetupResolver(); // Unused
      void tfSetupInterpreter();
      void tfAllocateTensors();
      void tfSetupInput();

      

      
  };
}

#endif //TF_LITE_WRAPPER_H
