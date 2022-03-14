const int BAUDRATE = 115200;

// Insert your network credentials
#define WIFI_SSID "MyWiFi_S9"
#define WIFI_PASSWORD "HelpUsObiWan"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDcEfTiFa-gRUBW0VEK099CaUaWPaa24aE"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "universidad.pedroni@gmail.com"
#define USER_PASSWORD "HanSolo77"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://esp-estacion-meteorologica-default-rtdb.firebaseio.com"

// Variables for storing data
String tempPath = "/temperature";
String humPath = "/humidity";
String voltPath = "/voltage";
String timePath = "/timestamp";

String parentPath; // Parent Node (to be updated in every loop)

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient (ntpUDP,"pool.ntp.org");
int timestamp;      // time stamp, epoch time. https://randomnerdtutorials.com/epoch-unix-time-esp8266-nodemcu-arduino/

FirebaseJson json;

// variables del sensor
float temperature;
float humidity;
float voltage;

// Constantes del sensor DHT11
#define DHTTYPE DHT11
const int DHT_PIN = 2;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

// Constantes de la pantalla oLed
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 32;
#define OLED_RESET -1
#define SCREEN_ADDRESS  0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32