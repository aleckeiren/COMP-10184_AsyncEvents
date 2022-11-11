/**
  I, Alec Pasion, 000811377 certify that this material is my original work. No other person's work has been used
  without due acknowledgement.
**/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
// pin assignments for external LEDs 
#define PIN_LED_GREEN   D1 
#define PIN_LED_YELLOW  D2 
#define PIN_LED_RED     D3 

// pin assignment for PIR input 
#define PIN_PIR         D5  
char ssid[] = "Mohawk-IoT";   // your network SSID (name) 
char pass[] = "IoT@MohawK1";   // your network password
WiFiClient  client;
unsigned long myChannelNumber = 1931952;
const char * myWriteAPIKey = "2AMKDCWJZ4X6R2SP";
volatile bool bEventOccured;
bool bPIR;
int stateCheck = 0;
int sensorState = 0;
long startTime = 0;
bool onRed = false;
long currentTime = millis();
long lastWrite = millis();
volatile bool writeToThinkSpeak = false;
void IRAM_ATTR interrupt(){
  bEventOccured = true;
  if(lastWrite + 10000 < millis()){
      writeToThinkSpeak = true;
  }else{
    lastWrite = millis();
  }
}

void setup() {
  Serial.begin(115200);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(PIN_PIR, INPUT); 

  pinMode(PIN_LED_GREEN, OUTPUT); 
  pinMode(PIN_LED_YELLOW, OUTPUT); 
  pinMode(PIN_LED_RED, OUTPUT); 
 
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
} 
 
void loop() {
  if(WiFi.status() != WL_CONNECTED){
    digitalWrite(PIN_LED_GREEN, LOW); 
    digitalWrite(PIN_LED_YELLOW, HIGH); 
    digitalWrite(PIN_LED_RED, LOW); 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    };
    Serial.printf("\nConnected.", WiFi.localIP().toString().c_str());
    
  }else{
    digitalWrite(PIN_LED_GREEN, HIGH); 
    digitalWrite(PIN_LED_YELLOW, LOW); 
    bPIR = digitalRead(PIN_PIR);
    attachInterrupt(digitalPinToInterrupt(PIN_PIR), interrupt, RISING);
  }
  currentTime = millis();
  if(bEventOccured){
    onRed = true;
    startTime = millis();
    if(writeToThinkSpeak){
      ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey);
      writeToThinkSpeak = false;
      Serial.println("Writing to TS");
    }
    bEventOccured = false;
  }
  
  if(onRed){
    digitalWrite(PIN_LED_RED, HIGH);
    if(currentTime > startTime +2000){
      onRed = false;
    }
  }else{
    digitalWrite(PIN_LED_RED, LOW);
  }
}