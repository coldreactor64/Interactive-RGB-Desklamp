
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "FastLED.h"
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <EEPROM.h>
#include "WebSocketsServer.h"

//Local Wifi Connection Info
const char *STA_ssid = "Hall-Wifi";
const char *STA_password = "crispytrail748";
//Access point for the Desk Lamp
const char *AP_ssid = "Desklamp";
const char *AP_password = "DeskLamp";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

/**RGB LED Configuration**/
#define DATA_PIN 4           //ESP8266 Pin Number
#define LED_TYPE WS2812      //LED Type
#define COLOR_ORDER GRB      //Order of Color on RGB Strip
#define NUM_LEDS 29          //Number of LEDs in strip
#define MILLI_AMPS 1900      //Set Maximum Current of the LEDs
#define FRAMES_PER_SECOND 30 // FPS (default: 120)

CRGB leds[NUM_LEDS];
CRGB Color = CRGB(244, 244, 244);

int power = 1;
uint8_t brightness = 255;
int timeout = 0;
bool liveData = false;
void setup()
{
  Serial.begin(115200);
  fastLEDSetup();
  int startupWifi = startupSTA();
  startmDNS();
  setupEndpoints();
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

/**
 * fastLEDSetup - Initializes FastLED Library according to settings
 */
void fastLEDSetup()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(brightness);
  fill_solid(leds, NUM_LEDS, Color);
  FastLED.show();
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
      Serial.printf("\n Could not Connect to %s \n", STA_ssid);
      return timeout;
    }
  }
  Serial.printf("\n Connected to %s \n", STA_ssid);
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

void loop()
{
  MDNS.update();
  server.handleClient();
  webSocket.loop();
  if (liveData == true)
  {
    EVERY_N_MILLISECONDS(1000 / (FRAMES_PER_SECOND))
    {
      sendLEDStatus();
      sinelon();
    }
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(0, 255, 192);
}

/**
 * writeCurrentStatus - writes current Lamp State to EEPROM
 * returns bool; 
 * */
bool writeCurrentStatus()
{
}

/**
 * setColor - Sets a Solid Color
 * */
void setColor(uint8_t r, uint8_t g, uint8_t b)
{
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
}

void setSingleColor(uint8_t LED, uint8_t r, uint8_t g, uint8_t b)
{
  leds[LED] = CRGB(r, g, b);
}

void setBrightness(int newBrightness)
{
  int value = map(newBrightness, 0, 100, 0, 255); //Map it to full brightness
  if (value > 255)
    value = 255;
  else if (value < 0)
    value = 0;

  FastLED.setBrightness(value);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  const char onLiveData[] = "on";
  const char offLiveData[] = "off";
  const char readyData[] = "ready";
  if (type == WStype_TEXT)
  {
    if (strcmp(onLiveData, (const char *)payload) == 0)
    {
      Serial.println("LiveData ON");
      liveData = true;
    }
    if (strcmp(offLiveData, (const char *)payload) == 0)
    {
      Serial.println("LiveData Off");
      liveData = false;
    }
    else
    {
      //webSocket.broadcastTXT(String(*payload));
    }
  }
}

void sendLEDStatus()
{
  String json = "{";
  json += "\"leds\":";
  json += "[";
  for (uint8_t i = 0; i < NUM_LEDS; i++)
  {
    json += "[";
    json += String(leds[i].red);
    json += ",";
    json += String(leds[i].green);
    json += ",";
    json += String(leds[i].blue);
    json += "]";
    if (i < NUM_LEDS - 1)
    {
      json += ",";
    }
  }
  json += "]";
  json += "}";
  webSocket.broadcastTXT(json);
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void setupEndpoints()
{
  server.on("/all", HTTP_GET, []() {
    sendAll();
  });

  server.on("/ip", HTTP_GET, []() {
    sendIP();
  });

  server.on("/power", HTTP_POST, []() {
    Serial.println("Power Command");
    //Grab the value out of the Post
    String value = server.arg("value");
    power = value.toInt();
    Serial.print(value);
    //Normal Operation
    if (power == 1)
    {
      Serial.println("Turn On");
      setBrightness(brightness);
    }
    if (power == 0)
    {
      Serial.println("Turn Off");
      //Make the old brightness stored in case we want to turn it back on
      setBrightness(0);
    }

    //Edge cases in case something messes up
    if (power > 1)
    {
      Serial.println("Error 1");
      setBrightness(brightness);
    }
    if (power < 0)
    {
      Serial.println("Error 2");
      setBrightness(0);
    }
    String json = "{";
    json += "\"power\":" + String(power);
    json += "}";
    server.send(200, "text/json", json);
  });

  server.on("/singleLED", HTTP_POST, []() {
    String r = server.arg("r");
    String g = server.arg("g");
    String b = server.arg("b");
    String led = server.arg("led");
    setSingleColor(led.toInt(), r.toInt(), g.toInt(), b.toInt());
    sendAll();
  });

  server.on("/solidColor", HTTP_POST, []() {
    String r = server.arg("r");
    String g = server.arg("g");
    String b = server.arg("b");
    setColor(r.toInt(), g.toInt(), b.toInt());
    sendAll();
  });

  server.on("/brightness", HTTP_POST, []() {
    String value = server.arg("value");
    Serial.println("Set Brightness");
    server.send(200);
    brightness = value.toInt();
    setBrightness(value.toInt());
  });

  server.on("/effect", HTTP_POST, [](){
      String effect = server.arg("effect");
      String settings = server.arg("settings");
      server.send(200);
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

void sendIP()
{
  IPAddress ip = WiFi.localIP();
  String localip = WiFi.localIP().toString().c_str();
  String json = "{";
  json += "\"ip\":" + String(" \"");
  json += String(localip);
  json += String("\"");
  json += "}";
  server.send(200, "text/json", json);
  json = String();
}
