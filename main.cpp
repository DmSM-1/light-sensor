#include <Arduino.h>
#include <WiFi.h>
#include "FS.h"
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <list>
#include <vector>
#include <string>

TaskHandle_t DetectorHandle;
std::vector<int> my_timer;
std::vector<int> level;
std::vector<int> level2;

unsigned long startTime = 0;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#define printled(a,b,c,d,e,f,g){\
    digitalWrite(GPIO_NUM_13, a);\
    digitalWrite(GPIO_NUM_12, b);\
    digitalWrite(GPIO_NUM_14, c);\
    digitalWrite(GPIO_NUM_27, d);\
    digitalWrite(GPIO_NUM_26, e);\
    digitalWrite(GPIO_NUM_25, f);\
    digitalWrite(GPIO_NUM_33, g);\
    delay(500);\
};


bool leds[8];

void IRAM_ATTR SP_MESSAGE(){
  portENTER_CRITICAL_ISR(&timerMux);
  unsigned long num = 0;
  my_timer.clear();
  level.clear();
  level2.clear();
  printled(1,0,0,0,1,0,0);
}


void Detector(void* parameters){
  startTime = millis();

  pinMode(GPIO_NUM_34, INPUT);
  unsigned long num = 0;
  unsigned long arr[3] = {0,0,0};
  char count = 0;

  while(true){
    num = 0;
    
    for(int i = 0; i < 1024; ++i){
      num += analogRead(GPIO_NUM_34);
    }
    if (count % 8 == 0)
      my_timer.push_back((int)(millis() - startTime)/1000);

    num /= 1024;
    Serial.println(num);
    if (count % 8 == 0)
      level.push_back(num);

    num /= 250;
    num = 9 - num;
    
    if (count % 8 == 0)
      level2.push_back(num);

    if (num == 9){
      leds[0] = 0;
      leds[1] = 1;
      leds[2] = 1;
      leds[3] = 1;
      leds[4] = 1;
      leds[5] = 1;
      leds[6] = 1;
    }
    else if (num == 8){
      leds[0] = 1;
      leds[1] = 1;
      leds[2] = 1;
      leds[3] = 1;
      leds[4] = 1;
      leds[5] = 1;
      leds[6] = 1;
    }
    else if (num == 7){
      leds[0] = 0;
      leds[1] = 0;
      leds[2] = 1;
      leds[3] = 0;
      leds[4] = 0;
      leds[5] = 1;
      leds[6] = 1;
    }
    else if (num == 6){
      leds[0] = 1;
      leds[1] = 1;
      leds[2] = 1;
      leds[3] = 1;
      leds[4] = 1;
      leds[5] = 0;
      leds[6] = 1;
    }
    else if (num == 5){
      leds[0] = 0;
      leds[1] = 1;
      leds[2] = 1;
      leds[3] = 1;
      leds[4] = 1;
      leds[5] = 0;
      leds[6] = 1;
    }
    else if (num == 4){
      leds[0] = 0;
      leds[1] = 0;
      leds[2] = 1;
      leds[3] = 1;
      leds[4] = 1;
      leds[5] = 1;
      leds[6] = 0;
    }
    else if (num == 3){
      leds[0] = 0;
      leds[1] = 1;
      leds[2] = 1;
      leds[3] = 0;
      leds[4] = 1;
      leds[5] = 1;
      leds[6] = 1;
    }
    else if (num == 2){
      leds[0] = 1;
      leds[1] = 1;
      leds[2] = 0;
      leds[3] = 0;
      leds[4] = 1;
      leds[5] = 1;
      leds[6] = 1;
    }
    else if (num == 1){
      leds[0] = 0;
      leds[1] = 0;
      leds[2] = 1;
      leds[3] = 0;
      leds[4] = 0;
      leds[5] = 1;
      leds[6] = 0;
    }
    else{
      leds[0] = 1;
      leds[1] = 1;
      leds[2] = 1;
      leds[3] = 1;
      leds[4] = 0;
      leds[5] = 1;
      leds[6] = 1;
    }


    digitalWrite(GPIO_NUM_13, leds[0]);
    digitalWrite(GPIO_NUM_12, leds[1]);
    digitalWrite(GPIO_NUM_14, leds[2]);
    digitalWrite(GPIO_NUM_27, leds[3]);
    digitalWrite(GPIO_NUM_26, leds[4]);
    digitalWrite(GPIO_NUM_25, leds[5]);
    digitalWrite(GPIO_NUM_33, leds[6]);
    count++;
    delay(2000);
}
}

const char* ssid = "user1";
const char* password = "123456789";
AsyncWebServer server(80);

void WebServer(){

  Serial.println("WebServer");
  
WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();

    Serial.print("IP: ");
    vTaskDelay(1000);
    Serial.println(IP);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String s = 
"<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <title>Яркость</title>"
"    <style>"
"        body {"
"            font-family: Arial, sans-serif;"
"            background-color: #111;"
"            color: #eee;"
"            margin: 0;"
"            padding: 20px;"
"        }"
"        h1 {"
"            color: #ffca28;"
"            text-align: center;"
"        }"
"        table {"
"            width: 90%;"
"            margin: 20px auto;"
"            border-collapse: collapse;"
"        }"
"        th, td {"
"            padding: 15px;"
"            text-align: left;"
"            border-bottom: 1px solid #444;"
"        }"
"        th {"
"            background-color: #333;"
"        }"
"        tr:nth-child(even) {"
"            background-color: #222;"
"        }"

"    </style>"
"</head>"
"<body>"
"    <h1>Яркость</h1>"
"    <div>"
"<p style=\"font-size: 18px; font-weight: bold;\">Яркость: <span id=\"brightness\" style=\"color: #ffcc00;\">";
s+=level.back();
s+="</span></p>"
"<button class=\"button\" onclick=\"location.reload();\" style=\"background-color: #ffcc00; margin-right: 20px; padding: 10px 20px;\">Обновить</button>"
"<style>"
"    .button {"
"        background-color: #ffcc00;"
"        border: none;"
"        color: black;"
"        padding: 10px 20px;"
"        text-align: center;"
"        text-decoration: none;"
"        display: inline-block;"
"        font-size: 14px;"
"        cursor: pointer;"
"        border-radius: 5px;"
"    }"
""
"    .button:hover {"
"        background-color: #d4aa00;"
"    }"
"</style>"
"    </div>"
"    <table>"
"        <thead>"
"            <tr>"
"                <th>Время</th>"
"                <th>Яркость</th>"
"                <th>Уровень</th>"
"            </tr>"
"        </thead>"
"        <tbody id=\"brightnessData\">";

for(int i = 0; i < my_timer.size() ; i++){
  s+= "<tr>";
  s+= "<td>";
  s+= my_timer[i];
  s+= "</td>";

  s+= "<td>";
  s+= level[i];
  s+= "</td>";


  s+= "<td>";
  s+= level2[i];
  s+= "</td>";
  s+= "</tr>";
}

s+=
"        </tbody>"
"    </table>"
"</body>"
"</html>";
        request->send(200, "text/html", s);
        ;});
    server.begin();

}

void setup() {
  Serial.begin(115200);

  pinMode(GPIO_NUM_13, OUTPUT);
  pinMode(GPIO_NUM_12, OUTPUT);
  pinMode(GPIO_NUM_14, OUTPUT);
  pinMode(GPIO_NUM_27, OUTPUT);
  pinMode(GPIO_NUM_26, OUTPUT);
  pinMode(GPIO_NUM_25, OUTPUT);
  pinMode(GPIO_NUM_33, OUTPUT);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &SP_MESSAGE, true);
  timerAlarmWrite(timer, 120000000, true);
  timerAlarmEnable(timer);

  
  WebServer();
  xTaskCreatePinnedToCore(Detector, "Task1", 10000, NULL, 0, &DetectorHandle, 0);

}

void loop() {
}
