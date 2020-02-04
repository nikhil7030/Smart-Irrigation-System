/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MITsssssssssss license
  This example code is in public domain.

 *************************************************************
  This example runs directly on NodeMCU.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right NodeMCU module
  in the Tools -> Board menu!

  For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//#include<DHT.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

#define trigger D0
#define echo D1
#define BLYNK_PRINT Serial  
WiFiClient client;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "LwDezzeJ04Ed94KC8yeGW3_S8lZERfFs";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Monika";
char pass[] = "gotiitdone";

int r1= 16; // Motor relay On PIN D0
int sense_Pin= 0; // Soil Sensor input at Analog PIN A0
int value= 0;
int DRY= 2;  // Dry Indicator at Digital PIN D4

BlynkTimer timer;

String weatherMain = "";
String weatherDescription = "";
String weatherLocation = "";
String country;
int humidity;
int pressure;
int temp;
int tempMin, tempMax;
int clouds;
int windSpeed;
String weatherString;

String weatherKey  = "d270e7c09e1c5d95f3edeed70fc5940b";   // openweathermap.org key
String weatherLang = "&lang=en";  //  english or other language
const char *weatherHost = "api.openweathermap.org";

void setup()
{
  // Debug console
  Serial.begin(9600);  //Starts sending data with parameter of BitRate

  Blynk.begin(auth, ssid, pass); 
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

   
   pinMode(DRY, OUTPUT);  //
   pinMode(echo, INPUT);  //Input Of HCHR04
   delay(2000);
   timer.setInterval(1000L,sendvalues); //Interval Between Method Call 1000ms
   
}

void loop()
{
  Blynk.run();  
  timer.run();  //  Starting Timer 

  long duration, distance; 
  duration = pulseIn(echo, HIGH);   //Taking ultrasonic Input
  distance = (duration/2) / 29.1;   //Calculation Distance Upon Signal Duration
  Blynk.virtualWrite(V2,distance);  //Updating Water level
}

void sendvalues()
{
   Serial.print("MOISTURE LEVEL : ");   
   value = analogRead(sense_Pin);   //Taking Analogue value
   value = value/10;    // Calculate The Percentage
   Serial.println(value);   
   if(temp <= 30 && temp >= 23 && value<40)
   {
      digitalWrite(r1, HIGH);   //Turn Motor On
   }
   else if(temp <= 40 && temp >= 30 && value<60)
   {
      digitalWrite(r1, HIGH);   //Turn Motor On
   }
   else
   {
      digitalWrite(r1,LOW);     //Turn Motor Off
   }

   Blynk.virtualWrite(V1,value);    //Update Value
}


void getWeatherData()     //API Call Method
{
  Serial.print("connecting to "); 
  Serial.println(weatherHost);
  delay(4000);
  if (client.connect(weatherHost, 80)) //If API Connects Succesfully
  {
    client.println(String("GET /data/2.5/weather?q=panvel,IN&") + "&units=metric&appid=" + weatherKey + weatherLang + "\r\n" +
                "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                "Connection: close\r\n\r\n");
  } 
  else 
  {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) //Returns the number of bytes available for reading
  {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  while (client.connected() && client.available()) //Checks Connection And Available Bytes
  {
    char c = client.read(); 
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }
  
  client.stop();

  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  weatherDescription = root["weather"]["description"].as<String>();
  weatherDescription.toLowerCase();
  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  tempMin = root["main"]["temp_min"];
  tempMax = root["main"]["temp_max"];
  windSpeed = root["wind"]["speed"];
  clouds = root["clouds"]["all"];
}
