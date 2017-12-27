/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "nscanf.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include "password.h"

#define USE_SERIAL Serial
#define BUFF_SIZE 2048
#define LED_NUM 60
#define LED_PIN1 5
#define LED_PIN2 4
char tmp[BUFF_SIZE] = {0};
ESP8266WiFiMulti WiFiMulti;
uint32_t up_line_pix[LED_NUM] = {0};
uint32_t down_line_pix[LED_NUM] = {0};
Adafruit_NeoPixel up_line_led =   Adafruit_NeoPixel(LED_NUM, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel down_line_led = Adafruit_NeoPixel(LED_NUM, LED_PIN2, NEO_GRB + NEO_KHZ800);
Ticker timer;

void timer_event(){
  static bool toggle = true;
  if(toggle){
    for(int i = 1; i < LED_NUM; i +=2){
      up_line_led.setPixelColor(i,up_line_pix[i]);
      down_line_led.setPixelColor(i,down_line_pix[i]);
    }
    toggle = false;
  }else{
    for(int i = 1; i < LED_NUM; i +=2){
      up_line_led.setPixelColor(i,0);
      down_line_led.setPixelColor(i,0);
    }
    toggle = true;
  }
  up_line_led.show();
  down_line_led.show();
}
void setup() {
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
    USE_SERIAL.begin(115200);
   //USE_SERIAL.setDebugOutput(true);
    up_line_led.begin();
    down_line_led.begin();
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    //timer.attach_ms(100,timer_event);
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("7308", wifi_password);

}
void parseCSV(char* str,int size){
  timer.detach();
  char *p;
  int cnt = 0;
  int r1,g1,b1,r2,g2,b2;
  p = strtok(str,"\n");
  while(p){
    nscanf(p,"%d,%d,%d,%d,%d,%d",&r1,&g1,&b1,&r2,&g2,&b2);
    Serial.printf("r,g,b : %d,%d,%d r,g,b : %d,%d,%d\n",r1,b1,g1,r2,g2,b2);
    up_line_led.setPixelColor(cnt,r1,g1,b1);
    down_line_led.setPixelColor(cnt,r2,g2,b2);
    //up_line_led.setPixelColor(cnt,5,0,0);
    //down_line_led.setPixelColor(cnt,0,5,0);
    cnt ++;
    if(cnt > LED_NUM -1 )break;
    p = strtok(NULL,"\n");
  }
  //up_line_led.setPixelColor(0,0,10,10);
  //down_line_led.setPixelColor(0,10,10,0);
  up_line_led.show();
  delay(100);
  down_line_led.show();
  for(int i = 0; i < LED_NUM; i ++){
    up_line_pix[i] = up_line_led.getPixelColor(i);
    down_line_pix[i] = down_line_led.getPixelColor(i);
  }
  for(int i = 0; i < size; i ++){
    //きれいにしておく
    tmp[i] = 0;
  }
  timer.attach_ms(500,timer_event);
}
void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://172.104.92.232:4567/l/nanbu"); //HTTP
        
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                //.println(payload.length);
                //USE_SERIAL.println(payload);
                payload.toCharArray(tmp,BUFF_SIZE);
                parseCSV(tmp,BUFF_SIZE);
                //Serial.println(tmp);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(30000UL);
}

