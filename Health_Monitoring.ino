#define IP "184.106.153.149"      // thingspeak.com ip
#include <SoftwareSerial.h>
#include "Timer.h"
#include <PulseSensorPlayground.h>

#include <Event.h>
#include <Timer.h>

#define WIFI_ID "####"     // "SSID-WiFiname"
#define WIFI_PASSWORD "###" // "password"

Timer tim;
PulseSensorPlayground pulseSensor;

String ServerAPI = "GET /update?key=#####";  // ThingSpeak channel API key  
SoftwareSerial esp8266(10,11);

const int PW = A0;    
const int ind = 13; 
int Th = 550; 
float LM35_temp;
int Pulse_BPM;

String Pulse_BPM_str;
String LM35_temp_str;

int Temp_raw;
float V,Celsius;

#define USE_ARDUINO_INTERRUPTS true    
#define DEBUG true
//////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600); 
  esp8266.begin(115200); 
         
  pulseSensor.setThreshold(Th);   
  pulseSensor.blinkOnPulse(ind);
  pulseSensor.analogInput(PW);  

  if (pulseSensor.begin()) {
    Serial.println("Success!!!");  
  }
  
  Serial.println("AT");
  esp8266.println("AT");

  delay(3000);

  if(esp8266.find("OK"))
  {
    WiFiConnection();
  }
  tim.every(1000, getTemp);
  tim.every(1000, getPulse);
  tim.every(1000, SendData);
}
/////////////////////////////////////////////////////////
void loop()
{
  tim.SendData(); 
  delay(3000);
}
///////////////////////////////////////////////////////////////////
void getPulse(){
  Pulse_BPM = pulseSensor.getBeatsPerMinute();                                  
	if (pulseSensor.sawStartOfBeat()) {         
	Serial.print("BPM:");
	Serial.println(Pulse_BPM);                      
	}

    delay(20);            
    char b1[10];
    Pulse_BPM_str = dtostrf(Pulse_BPM, 4, 1, b1);  
}
///////////////////////////////////////////////////////////////////
void getTemp()
{
  Temp_raw = analogRead(A1);
  V = (Temp_raw / 1023.0) * 500; // 5000 to get millivots.
  Celsius = V * 0.1; 
  
  LM35_temp = (Celsius * 1.8) + 32; // conver to F
  Serial.print("Fahrenheit:");
  Serial.println(LM35_temp);
  
  delay(20);
  char b2[10];
  LM35_temp_str = dtostrf(LM35_temp, 4, 1, b2);
}

///////////////////////////////////////////////////////////////////
boolean WiFiConnection()
{
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String command="AT+CWJAP=\"";
  command+=WIFI_ID;
  command+="\",\"";
  command+=WIFI_PASSWORD;
  command+="\"";
  Serial.println(command);
  esp8266.println(command);
  delay(5000);
  
  if(esp8266.find("OK"))
  {
    Serial.println("*********WIFI CONNECTED************");
    return true;
  }
  else
  {
    return false;
  }
}
//////////////////////////////////////////////////////////
void SendData()
{
  String command = "AT+CIPSTART=\"TCP\",\"";
  command += IP;
  command += "\",80";
  Serial.println(command);
  esp8266.println(command);
  delay(2000);
  
  if(esp8266.find("Error"))
  {
    return;
  }
  command = ServerAPI ;
  command += "&field1="; 
  command += Pulse_BPM_str;
  command += "&field2=";  
  command += LM35_temp_str;
  command += "\r\n";
  
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  
  Serial.println(command.length());
  esp8266.println(command.length());
  
  if(esp8266.find(">"))
  {
    Serial.print(command);
    esp8266.print(command);
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    esp8266.println("AT+CIPCLOSE");
  }
}
/////////////////////////////////////////////////
