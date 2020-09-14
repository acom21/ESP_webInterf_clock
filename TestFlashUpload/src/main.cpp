
//iotClock Lite WifiManager version v.18.11.12
//Simple wifi clock on Wemos D1 mini ESP8266 module
//also compatible with NodeMCU, ESP12 etc.
//some code elements borrowed from clock project in Phillipines
//written by iottaman aka BenBen benben@iottoman.online
//===============================================================
// LED Matrix Pins -> Wemos D1 mini and NodeMCU 
// pins are configured to be compatible with IvanUA WiFi clock 
// Vcc -> 5V pin
// Gnd -> Gnd pin 
// DIN -> D7 Pin 
// CS  -> D8 Pin 
// CLK -> D5 Pin 
#include <Arduino.h>

#include <SPI.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

//This library can be found here: https://github.com/markruys/arduino-Max72xxPanel
//Download arduino-Max72xxPanel-master.zip file, extract included folder into Arduino/libraries and rename it to  Max72xxPanel


//LED display CS pin 
//if connected to D4 it will show display activity on Wemos built-in LED
//int pinCS = D4;
int pinCS = D8;
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
char time_value[20];
//set your timezone here: UTC = 0, Russia MSK = 3, Ukraine = 3 summer/2 winter
int timeZone = 3;//UTC
int matrixBrightness = 4;//Use a value between 0 and 15 for brightness

// Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int scroll_wait = 40; //Scroll speed  - dot shift delay in milliseconds
int scroll_string_timer = 15; //Scroll string interval in seconds
int scrollCycle = 1; //varible for 1sec counter in loop
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels
int ax = 0, ax1, ay, tick2 = 0; //variables to control animation
//====================================================================
void setup() {
  Serial.begin(115200);
  delay(10);Serial.println("\nSetup started");
  //initialize display
  // matrix.setIntensity(matrixBrightness); 
  // matrix.setRotation(0, 1); //set matrix positions
  // matrix.setRotation(1, 1);
  // matrix.setRotation(2, 1);
  // matrix.setRotation(3, 1);
  // matrix.fillScreen(LOW);matrix.write();
 // display_message("iotClock WiFi start");
  WiFiManager wifiManager; // WiFi manager object for WiFi configuration via browser
  int wifiConnTime = millis();
  while (WiFi.status() != WL_CONNECTED) { 
    //WiFi connection animation
    // matrix.fillScreen(LOW);
    // matrix.drawRoundRect(matrix.width()/2-ax*4, matrix.height()/2-ax, 8*ax, 2*ax, ax/2, HIGH);
    // matrix.write(); 
    // if(ax++ > 4) ax = 0;
    // if(ax1++ > 28){
    //   matrix.fillScreen(LOW);
    //   display_message("no WiFi connect to iotClock and go to 192.168.4.1");
    //   matrix.setCursor(0,0);
    //   matrix.print("WiFi!");
    //   matrix.write(); // Send bitmap to display     
      wifiManager.autoConnect("iotClock");
    //   Serial.println("No WiFi connection, WiFi manager started");
    //   ax1 = 0; 
    }
    delay(500);
  }
  wifiConnTime = millis() - wifiConnTime;
  String wifiInfo = "Connected to " + String(WiFi.SSID()) + " in " + String(float(wifiConnTime)/1000,1) + " Sec WiFi signal:" + String(WiFi.RSSI());
  wifiInfo += " dBm IP:" + WiFi.localIP().toString();
  Serial.println();Serial.println(wifiInfo);Serial.print("WiFi connection status:");Serial.println(WiFi.status());
  // matrix.fillScreen(LOW);
  // display_message(wifiInfo);
  
  //Configure time server connection
   ax = 0;//set animation variable and start Internet time sync 
   configTime(3600*timeZone, 0, "pool.ntp.org", "time.nist.gov");
   Serial.println("Waiting for time");
   Serial.println(time(nullptr));
  // //if(time(nullptr) <= 100000) display_message("Checking time");
   while (time(nullptr) <= 100000) {
  //   //time server connection animation
  //   matrix.fillScreen(LOW);
  //   for(ay = 0;ay <= 3;ay++){
  //     matrix.drawCircle(float(ay*8+matrix.width()/16) + 1, float(matrix.height()/2), ax, HIGH);
  //   }
  //   matrix.write(); 
  //   if(ax++ > 2) ax = 0;
  //   if(ax1++ > 23){
  //     matrix.fillScreen(LOW);
  //     display_message("No Internet time");
    Serial.println("No time server connection, check Internet");
  //     ax = 0;ax1 = 0; 
  //   }
    delay(500);
    }  
   Serial.println("Time server connected");
  // }

//====================================================================
void loop() {
  //matrix.fillScreen(LOW);
  time_t now = time(nullptr);
  String timeStr = String(ctime(&now));
   timeStr.trim();
   timeStr.substring(11, 19).toCharArray(time_value, 10);
  // matrix.drawChar(2, 0, time_value[0], HIGH, LOW, 1); // H
  // matrix.drawChar(8, 0, time_value[1], HIGH, LOW, 1); // HH
  // matrix.drawChar(14, 0, time_value[2], tick2, LOW, 1); // HH:
  // matrix.drawChar(20, 0, time_value[3], HIGH, LOW, 1); // HH:M
  // matrix.drawChar(26, 0, time_value[4], HIGH, LOW, 1); // HH:MM
  // matrix.write(); // Send bitmap to display
  // tick2 = !tick2;
  // delay(1000); //main clock loop cycle
  // //clear display
  // matrix.fillScreen(LOW);
  // //check WiFi connection
  if(WiFi.status() != WL_CONNECTED) timeStr += " No WiFi!";
  if(scrollCycle++ >= scroll_string_timer){
   // display_message(timeStr); // Display time in format 'Wed, Nov 07 22:03:20 2018
    Serial.println(timeStr);
    scrollCycle = 1;
  }
}

// void display_message(String message) {
//   for ( int i = 0 ; i < width * message.length() + matrix.width() - spacer; i++ ) {
//     //matrix.fillScreen(LOW);
//     int letter = i / width;
//     int x = (matrix.width() - 1) - i % width;
//     int y = (matrix.height() - 8) / 2; // center the text vertically
//     while ( x + width - spacer >= 0 && letter >= 0 ) {
//       if ( letter < message.length() ) {
//         matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background OFF
//        }
//     letter--;
//     x -= width;
//     yield();
//     }
//   matrix.write(); // Send bitmap to display
//   delay(scroll_wait);
//   }
// }

