/* Estación meteorológica + Servidor online
https://randomnerdtutorials.com/esp8266-data-logging-firebase-realtime-database/
Simulador pantalla OLED (ya cableada)
https://wokwi.com/arduino/libraries/Adafruit_SSD1306/ssd1306_128x64_i2c
PINOUT ESP-07:
https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
*/


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"
#include "DHT.h"
#include "Streaming.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


// Firebase Objects and variables
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;   // user Id
String databasePath;


// DHT
DHT dht(DHT_PIN, DHTTYPE);
// Medición del voltaje de alimentación
ADC_MODE(ADC_VCC);
// DISPLAY OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initWiFi();
unsigned long getTime();
bool displayConnect();
void displayDatos(float temp, float hum, float volts, unsigned int now);
void initFirebase();

void setup() 
{
  Serial.begin(BAUDRATE);
  Serial.println();
  Serial << F("Online:\n") << __TIME__ << F("\n");
  pinMode(DHT_PIN,INPUT);
  dht.begin();
  initWiFi();
  timeClient.begin();
  
  if(displayConnect())
  {
    //testscrolltext();    // Draw scrolling text
     display.clearDisplay();

    display.setTextSize(1); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Hola Mundo"));
    display.println(F("Wifi Conectado"));
    display.println(WiFi.localIP());
    display.display();      // Show initial text
    delay(2000);
  }
  initFirebase();
}

void loop() 
{

  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    timestamp = getTime();
    parentPath = databasePath + "/" + String(timestamp);
    json.set(tempPath.c_str(), String(dht.readTemperature()));
    json.set(humPath.c_str(), String(dht.readHumidity()));
    json.set(voltPath.c_str(),String((float)ESP.getVcc() / 1000 ));
    json.set(timePath,String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    displayDatos(dht.readTemperature(),dht.readHumidity(),(float)ESP.getVcc() / 1000, timestamp);
  }
  
  
}

void initWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial << "Conectando a WiFi...\n";
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial << F(".");
    delay(1000);
  }
  Serial << F("Conectado\n");
  Serial.println(WiFi.localIP()) ;

}

unsigned long getTime()
{
    timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

bool displayConnect()
{
  bool connect = false;
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
  }
  else
  {
    Serial << F("SSD1306 is alive!\n");
    connect = true;
  }
  return connect;    
 
}

void displayDatos(float temp, float hum, float volts, unsigned int now)
{
  display.clearDisplay();
  
  display.setCursor(0,0);
  display.print(F("Temp: "));
  display.print(temp);
  display.println(F("C"));
  
  display.print(F("Hum: "));
  display.print(hum);
  display.println(F("%"));
  
  display.print(F("Volts: "));
  display.print(volts);
  display.println(F(" v"));
  
  display.print(F("Time: "));
  display.println(now);
  
  display.display();

}

void initFirebase()
{
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = TokenStatusCallback();
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  Serial << F("Getting user UID...\n");
  while ((auth.token.uid) == "")
  {
    Serial << F(".");
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial << F("User Id: ") << uid << F("\n");
  databasePath = "/UsersData/" + uid + "/readings";




}