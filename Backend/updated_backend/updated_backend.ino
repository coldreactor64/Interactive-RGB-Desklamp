
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <EEPROM.h>
#include "WebSocketsServer.h"
#include "led_effects.h"
#include "config.h"
//#include "mqtthelper.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
//Local Wifi Connection Info
const char *STA_ssid = "Hall-Wifi";
const char *STA_password = "crispytrail748";
//Access point for the Desk Lamp
const char *AP_ssid = "Desklamp";
const char *AP_password = "DeskLamp";
// Create an ESP8266 WiFiClient class to connect to the MQTT server.

WiFiClient client;

/**Adafruit MQTT Subscriptions and Publish objects**/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe onoffSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.state", MQTT_QOS_1);
Adafruit_MQTT_Subscribe brightnessSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.brightness", MQTT_QOS_1);
Adafruit_MQTT_Subscribe previousEffectSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.previouseffect", MQTT_QOS_1);
Adafruit_MQTT_Subscribe nextEffectSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.nexteffect", MQTT_QOS_1);

Adafruit_MQTT_Publish previousEffectPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.previouseffect", MQTT_QOS_1);
Adafruit_MQTT_Publish nextEffectPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.nexteffect", MQTT_QOS_1);

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
Led_effects *LED;

// /**RGB LED Configuration**/
// #define DATA_PIN 4           //ESP8266 Pin Number
// #define LED_TYPE WS2812      //LED Type
// #define COLOR_ORDER GRB      //Order of Color on RGB Strip
// #define NUM_LEDS 29          //Number of LEDs in strip
// #define MILLI_AMPS 1900      //Set Maximum Current of the LEDs
// #define FRAMES_PER_SECOND 30 // FPS (default: 120)

// CRGB leds[NUM_LEDS];
CRGB Color = CRGB(244, 244, 244);
int power = 1;

uint8_t startup_brightness = 40;
int timeout = 0;
bool liveData = false;
void setup()
{
  Serial.begin(115200);
  LED = new Led_effects(startup_brightness);
  int startupWifi = startupSTA();
  startmDNS();
  setupEndpoints();
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  MQTT_setup();
}

/**
 * fastLEDSetup - Initializes FastLED Library according to settings
 */
void fastLEDSetup()
{
  //TODO: replace with class initializer of led_effects
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(LED->leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(startup_brightness);
  fill_solid(LED->leds, NUM_LEDS, Color);
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
  EVERY_N_MILLISECONDS(1000 / (FRAMES_PER_SECOND))
  {
    LED->update_leds();
  }
  EVERY_N_MILLISECONDS(100)
  {
    MQTT_connect();
    mqtt.processPackets(20);
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

/**
 * writeCurrentStatus - writes current Lamp State to EEPROM
 * returns bool; 
 * */
bool writeCurrentStatus()
{
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
    json += String(LED->leds[i].red);
    json += ",";
    json += String(LED->leds[i].green);
    json += ",";
    json += String(LED->leds[i].blue);
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
      LED->set_brightness(LED->brightness);
    }
    if (power == 0)
    {
      Serial.println("Turn Off");
      //Make the old brightness stored in case we want to turn it back on
      LED->set_brightness(0);
    }

    //Edge cases in case something messes up
    if (power > 1)
    {
      Serial.println("Error 1");
      LED->set_brightness(LED->brightness);
    }
    if (power < 0)
    {
      Serial.println("Error 2");
      LED->set_brightness(0);
    }
    String json = "{";
    json += "\"power\":" + String(power);
    json += "}";
    server.send(200, "text/json", json);
  });

  server.on("/brightness", HTTP_POST, []() {
    String value = server.arg("value");
    Serial.println("Set Brightness");
    server.send(200);
    LED->brightness = value.toInt();
    LED->set_brightness(value.toInt());
  });

  server.on("/effect", HTTP_POST, []() {
    String effect = server.arg("effect");
    String settings = server.arg("settings");
    String returnData = LED->parse_effect(effect, settings);
    server.send(200);
  });
}

void sendAll()
{
  String json = "{";
  json += "\"power\":" + String(power) + ",";
  json += "\"brightness\":" + String(LED->brightness) + ",";
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

void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void onoffcallback(char *data, uint16_t len)
{
  const char onCommand[] = "on";
  const char offCommand[] = "off";
  if (strcmp(onCommand, (const char *)data) == 0)
  {
    Serial.println("Turn On");
    LED->set_brightness(LED->oldBrightness);
  }
  if (strcmp(offCommand, (const char *)data) == 0)
  {
    Serial.println("Turn Off");
    //Make the old brightness stored in case we want to turn it back on
    LED->oldBrightness = LED->brightness;
    LED->set_brightness(0);
  }
}

void brightnessCallback(char *data, uint16_t len)
{
  LED->brightness = atoi(data);
  LED->set_brightness(atoi(data));
  Serial.print(atoi(data));
}

void nextEffectCallback(char *data, uint16_t len)
{
  const char nextEffectCommand[] = "nexteffect";
  if (strcmp(nextEffectCommand, (const char *)data) == 0)
  {
    //TODO: Add next effect method in LED Class
    nextEffectPublish.publish("0");
  }
}

void previousEffectCallback(char *data, uint16_t len)
{
  const char previousEffectCommand[] = "previouseffect";
  if (strcmp(previousEffectCommand, (const char *)data) == 0)
  {
    //TODO: Add previous effect method in LED Class
    nextEffectPublish.publish("0");
  }
}

void MQTT_setup()
{
  mqtt.subscribe(&onoffSubscribe);
  onoffSubscribe.setCallback(onoffcallback);

  mqtt.subscribe(&brightnessSubscribe);
  brightnessSubscribe.setCallback(brightnessCallback);

  mqtt.subscribe(&nextEffectSubscribe);
  brightnessSubscribe.setCallback(previousEffectCallback);
}