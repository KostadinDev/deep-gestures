#include "output.h";
#include <Arduino.h>;

/**
 * output - array of output activation values
 * pinNums - array of output pins corresponding with activation values
 * len - number of outputs / pins
 */
 void handleOutput(float output[],int pinNums[], int len ){
   if(!Serial){
    Serial.begin(9600);
   }

    for( int i = 0; i < len; i++){
      Serial.print("Prediction ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(output[i]);
      
      if(output[0] > .5){
        analogWrite(pinNums[i],output[0]*255);
      }
      Serial.println("data done");
    }
    Serial.println("finished outputting");
    /*
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
    */
    //writeBLE(gesture_pred);

    /*
    Serial.println("1");
    if(gesture_pred[0] > .5){
      analogWrite(O_PIN,gesture_pred[0]*255);
      //digitalWrite(O_PIN, HIGH);
    }
    Serial.println("2");
    if(gesture_pred[1] > .5){
      analogWrite(LR_PIN,gesture_pred[1]*255);
      //digitalWrite(LR_PIN, HIGH);
    }
    Serial.println("3");
    if(gesture_pred[2] > .5){
      analogWrite(RL_PIN,gesture_pred[2]*255);
      //digitalWrite(RL_PIN, HIGH);
    }
    Serial.println("4");
    if(gesture_pred[3] > .5){
      analogWrite(SPIN_PIN,gesture_pred[3]*255);
      //digitalWrite(SPIN_PIN, HIGH);
    }
    Serial.println("5");
    if(gesture_pred[4] > .5){
      digitalWrite(THROW_PIN, HIGH);
    }
    */
    
    Serial.println("6");
    delay(1000);
    Serial.println("pins vvv");
    for(int i = 0; i < len; i++){
      Serial.println(pinNums[i]);
      analogWrite(pinNums[i],0);
      Serial.println("pin");
    }
    Serial.println("pins ^^");
    /*
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
    */

    Serial.println("finished");
 }
