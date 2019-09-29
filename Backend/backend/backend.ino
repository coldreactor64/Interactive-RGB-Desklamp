#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <EEPROM.h>

//Local Wifi Connection Info
const char *STA_ssid = "Hall-Wifi";
const char *STA_password = "crispytrail748";
//Access point for the Desk Lamp
const char *AP_ssid = "Desklamp";
const char *AP_password = "DeskLamp";

uint8_t brightness;
ESP8266WebServer server(80);
#define DATA_PIN 4 //Raw Pin Number 4
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 29
#define MILLI_AMPS 1900       // IMPORTANT: set here the max milli-Amps of your power supply 5V 2A = 2000
#define FRAMES_PER_SECOND 120 // here you can control the speed. With the Access Point / Web Server the animations run a bit slower.

CRGB leds[NUM_LEDS];//LED Current Array
CRGB Color = CRGB::Red;
int power = 1;
int timeout = 0;

void loadSettings()
{ //load the default settings and display them on startup

  brightness = EEPROM.read(0);
  byte r = EEPROM.read(1);
  byte g = EEPROM.read(2);
  byte b = EEPROM.read(3);

  if (r == 0 && g == 0 && b == 0)
  {
    Color = CRGB::Red;
  }
  else
  {
    Color = CRGB(r, g, b);
  }
  FastLED.setBrightness(brightness);
  fill_solid(leds, NUM_LEDS, Color);
  FastLED.show();
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(512); //Startup the EEPROM so I can read/write
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  loadSettings(); //load the current settings then set them
  //Try to Connect to Wifi, if not change to AP
  timeout = startupSTA();
  if (timeout == 60)
  {
    startupAP();
  }
  startmDNS();
  setupEndpoints();
  server.begin();
}

void loop()
{

  MDNS.update();
  server.handleClient();
  if (power == 0)
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    FastLED.delay(15);
    return;
  }
  FastLED.show();
}

void startupAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_ssid, AP_password);
  Serial.print("Started AP");
}

int startupSTA()
{
  WiFi.mode(WIFI_STA); //Change to Wifi Station Configuration
  Serial.printf("\n Connecting to %s \n", STA_ssid);
  //Attempt to connect to WiFi Network
  WiFi.begin(STA_ssid, STA_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    timeout++;
    Serial.print(".");
    if (timeout == 60)
    {
      return timeout;
    }
  }
  return timeout;
}

void startmDNS()
{
  if (!MDNS.begin("Desklamp"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
}

void setupEndpoints()
{
  server.on("/all", HTTP_GET, []() {
    Serial.print("Get All Status");
    sendAll();
  });

  server.on("/power", HTTP_POST, []() {
    Serial.println("Power Toggle");
    String value = server.arg("value");
    power = value.toInt();
    Serial.println(power);
    if (power > 1)
    {
      power = 1;
    }
  });

  /*
  server.on("/solidColor", HTTP_POST, []() {
    String r = server.arg("r");
    String g = server.arg("g");
    String b = server.arg("b");
    setColor(r.toInt(), g.toInt(), b.toInt());
    sendAll();
  });*/

  server.on("/brightness", HTTP_POST, []() {
    Serial.println("Assign Brightness");
    String value = server.arg("value");
    setBrightness(value.toInt());
    sendAll();
  });
}

void sendAll()
{
  String json = "{";
  json += "\"power\":" + String(power) + ",";
  json += "\"brightness\":" + String(brightness) + ",";
  json += "\"solidColor\":{";
  json += "\"r\":" + String(Color.r);
  json += ",\"g\":" + String(Color.g);
  json += ",\"b\":" + String(Color.b);
  json += "}";
  json += "}";
  server.send(200, "text/json", json);
  json = String();
}

void setBrightness(int value)
{
  if (value > 255)
    value = 255;
  else if (value < 0)
    value = 0;

  brightness = value;

  FastLED.setBrightness(brightness);

  EEPROM.write(0, brightness);
  EEPROM.commit();
}

void setColor(uint8_t r, uint8_t g, uint8_t b)
{ // Set a single solid color
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
}
