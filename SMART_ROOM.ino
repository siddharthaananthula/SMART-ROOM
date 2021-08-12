#define BLYNK_PRINT Serial
#include<LiquidCrystal_I2C.h> 
#include <DHT.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
#include <Wire.h>
#define in  D6 //PIR IN
#define out D7 //PIR OUT
int count=0;
#include <BlynkSimpleEsp8266.h>
#include"ThingSpeak.h"
#include<ESP8266WiFi.h>
#define DHTPIN D8  //DHT 
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
String apikey = "6MKZ7RYPUT51G686";
char auth[] = "aFD86ITQjLue1ooDILl-u6ZIzewkz9XV";
const char* ssid = "Jayaprakash";
const char* password = "9440029246";
const char* server = "api.thingspeak.com";
unsigned long channel = 1406324 ;
int field1 = 1;
int field2 = 2;
int field3 = 3;
int field4 = 4;
WiFiClient client;
void IN()
{
    count++;
    lcd.clear();
    lcd.print("Person In Room:");
    lcd.setCursor(0,1);
    lcd.print(count);
    delay(1000);
}

void OUT()
{
  count--;
    lcd.clear();
    lcd.print("Person In Room:");
    lcd.setCursor(0,1);
    lcd.print(count);
    delay(1000);
}
void setup()
{
   Wire.begin(2,0);
  lcd.init();  
  lcd.begin(16,2);
  lcd.print("SMART ROOM");
  delay(2000);
  pinMode(in, INPUT); 
  pinMode(out, INPUT);
  pinMode(D1,OUTPUT);  //LED
  pinMode(D2,OUTPUT); //FAN
  pinMode(D5,OUTPUT); //RELAY
  pinMode(D0,OUTPUT);  //BUZZER
  lcd.backlight();
  lcd.clear(); 
  lcd.print("Person In Room:");
  lcd.setCursor(0,1);
  lcd.print(count);
  Blynk.begin(auth, ssid, password);
  Serial.begin(115200);
  delay(100);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
 Serial.println();
 Serial.println();
 Serial.print("connecting to");
 Serial.println(ssid);
 WiFi.begin(ssid,password);
 while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
 }
 Serial.println("");
 Serial.println("wifi connected");
 Serial.println("ip address");
 Serial.println(WiFi.localIP());
}

void loop() 
{
      float h = dht.readHumidity();
      float t = dht.readTemperature();
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))  
                      {  
                            
                             String postStr = apikey;
                             postStr +="&field3=";
                             postStr += String(t);
                             postStr +="&field4=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apikey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
          Serial.println("Waiting...");
          delay(1000);
  int LED = ThingSpeak.readFloatField(channel,field1);
  int FAN = ThingSpeak.readFloatField(channel,field2);
  if(LED==1)
  {
    digitalWrite(D1,1);
    Serial.println("led is on");
  }
  else if(LED==0)
  {
    digitalWrite(D1,0);
    Serial.println("led is off");
  }
   if(FAN==1)
   {
    digitalWrite(D2,1);
    Serial.println("fan is on");
  }
  else if(FAN==0)
  {
    digitalWrite(D2,0);
    Serial.println("fan is off");
  }
  Serial.println(LED);
   Serial.println(FAN);
   delay(5000);
  Blynk.run();  
   if(digitalRead(in))
  IN();
  if(digitalRead(out))
  OUT();
  
  if(count<=0)
  {
    lcd.clear();
    digitalWrite(D5, LOW);
    lcd.clear();
    lcd.print("Nobody In Room");
    delay(200);
  }
  else
    {
      digitalWrite(D5, HIGH);
    }
  if(count>=4)
  {
    lcd.clear();
    digitalWrite(D0,HIGH);
    lcd.clear();
    lcd.print("Room overloaded");
    delay(200);
  }
}
