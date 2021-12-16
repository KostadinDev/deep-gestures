#include <Arduino_JSON.h>

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>


WiFiMulti wifiMulti;




String xStr ="";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  wifiMulti.addAP("mlink", "Maramara1");
}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;
  while(Serial2.available()){
    
    //Serial.print(Serial2.parseFloat(),4);
    xStr.concat(String(Serial2.parseFloat(),4));
    xStr.concat(",");
    Serial.println("");
  }
  if(xStr != ""){
    JSONVar myObject;
    myObject["x"] = xStr;
    myObject["y"] = xStr;
    myObject["z"] = xStr;
    Serial.println(JSON.stringify(myObject));
    sendData(JSON.stringify(myObject), 128);
  }
  xStr = "";
  delay(1);

}

void sendData(String body, unsigned int INPUT_LENGTH){
  if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        //http.begin("https://httpbin.org/anything ");
        http.begin("http://192.168.1.86:8000/submitgesture/");

        Serial.print("[HTTP] GET...\n");

        
        //http.addHeader("x", xStr, INPUT_LENGTH); 
        //http.addHeader("y", yStr, INPUT_LENGTH); 
        //http.addHeader("z", zStr, INPUT_LENGTH); 
        //int httpCode = http.POST(String(INPUT_LENGTH));
        int httpCode = http.POST(body);

        // httpCode will be negative on error
        if(httpCode > 0) {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                Serial.println(http.getString());
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    } else Serial.println("not connected");
}
