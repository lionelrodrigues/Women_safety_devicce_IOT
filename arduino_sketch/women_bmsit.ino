#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TinyGPS++.h>
#define heartratePin A0
#include "DFRobot_Heartrate.h"
#include <SoftwareSerial.h>
SoftwareSerial Gsm(5, 4);
SoftwareSerial Gps(14, 12);

WidgetMap myMap(V5);
TinyGPSPlus gps;
BlynkTimer timer;
DFRobot_Heartrate heartrate(DIGITAL_MODE);

char auth[] = "8fff08c769f44b81bd45f0cbcd807137";
char ssid[] = "";
char pass[] = "";

float latitude;
float longitude;
unsigned int move_index = 1;       // fixed location for now

void setup()
{
    Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
    Gsm.begin(9600);      // Setting the baud rate of GSM Module  
    Gps.begin(9600);      // Setting the baud rate of GPS Module  
    Blynk.begin(auth, ssid, pass);
    delay(100);
    timer.setInterval(200, sendUptime);
}
void sendUptime()
{
  uint8_t rateValue;
  heartrate.getValue(heartratePin); ///< A1 foot sampled values
  rateValue = heartrate.getRate(); ///< Get heart rate value 
  if(rateValue)  {
    Serial.println(rateValue);
    Blynk.virtualWrite(V0,rateValue);
  }
  if(rateValue > 90)
  {
    Blynk.notify("heart rate is high");
  }
}
void loop()
{
  if (Serial.available()>0)
   switch(Serial.read())
  {
    case 's':
      SendMessage();
      break;
  }
  while (Gps.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(Gps.read()))
        displayInfo();
  }
  Blynk.run();
  timer.run();
}


void SendMessage()
{
  Gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  Gsm.println("AT+CMGS=\"+918660412370\"\r"); // Replace x with mobile number
  delay(1000);
  Gsm.print("I NEED HELP \nhttp://maps.google.com/maps?q=loc:");
  Gsm.print(String(latitude,6) + "," + String(longitude,6));  
  delay(100);
  Gsm.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println("Sent sms!!!");
}
void SendMessage2()
{
  Gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  Gsm.println("AT+CMGS=\"+918660412370\"\r"); // Replace x with mobile number
  delay(1000);
  Gsm.print("I NEED HELP \nhttp://maps.google.com/maps?q=loc:");
  Gsm.print(String(latitude,6) + "," + String(longitude,6));  
  delay(100);
  Gsm.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println("Sent sms!!!");
}

void displayInfo()
{ 
  if (gps.location.isValid() ) 
  {
    latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    longitude = (gps.location.lng());
    myMap.location(move_index, latitude, longitude, "GPS_Location");   
  }
}
