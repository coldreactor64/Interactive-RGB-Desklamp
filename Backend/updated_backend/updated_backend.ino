
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <EEPROM.h>
#include "WebSocketsServer.h"
#include "led_effects.h"
#include "config.h"
#include "mqtthelper.h"
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
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
/**Adafruit MQTT Subscriptions and Publish objects**/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe onoffSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.onoff", MQTT_QOS_1);
Adafruit_MQTT_Subscribe brightnessSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.brightness", MQTT_QOS_1);
Adafruit_MQTT_Subscribe previousEffectSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.previouseffect", MQTT_QOS_1);
Adafruit_MQTT_Subscribe nextEffectSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.nexteffect", MQTT_QOS_1);
Adafruit_MQTT_Subscribe statusSubscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/desklamp.state", MQTT_QOS_1);

//Note: Anything published to /get suffix just returns the data to the client so you can use the callback
Adafruit_MQTT_Publish onOffGet = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.onoff/get", MQTT_QOS_1);
Adafruit_MQTT_Publish onOffPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.onoff", MQTT_QOS_1);
Adafruit_MQTT_Publish brightnessGet = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.brightness/get", MQTT_QOS_1);
Adafruit_MQTT_Publish brightnessPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.brightness", MQTT_QOS_1);
Adafruit_MQTT_Publish previousEffectPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.previouseffect", MQTT_QOS_1);
Adafruit_MQTT_Publish nextEffectPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.nexteffect", MQTT_QOS_1);
Adafruit_MQTT_Publish statusGet = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.state/get", MQTT_QOS_1);
Adafruit_MQTT_Publish statusPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/desklamp.state", MQTT_QOS_1);

Led_effects *LED;

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
  MQTT_connect();
}

void setupLamp()
{
  Serial.print("Setup Lamp");

  brightnessGet.publish(21312);
  onOffGet.publish(344);
  brightnessGet.publish(21312);
  statusGet.publish(1221);
}

/**
 * startup in station mode 
 */

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
    if (!mqtt.connected())
    {
      MQTT_connect();
    }
    mqtt.processPackets(10);
  }

  EVERY_N_MILLISECONDS(4000)
  {
    if (!mqtt.ping())
    {
      mqtt.disconnect();
    }
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
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

void setupEndpoints()
{
  server.on("/all", HTTP_GET, []() {
    send_Current_Status_Http();
  });

  server.on("/ip", HTTP_GET, []() {
    sendIP();
  });

  server.on("/power", HTTP_POST, []() {
    //Grab the value out of the Post
    String OnOffString = server.arg("value");
    char OnOffValue[OnOffString.length()];
    OnOffString.toCharArray(OnOffValue, OnOffString.length() + 1);
    //Normal Operation
    LED->set_power(OnOffValue);
    onOffPublish.publish(OnOffValue);
    String json = "{";
    json += "\"brightness\":" + String(LED->brightness);
    json += "}";
    server.send(200, "text/json", json);
  });

  server.on("/brightness", HTTP_POST, []() {
    String value = server.arg("value");
    Serial.println("Set Brightness");
    server.send(200);
    uint8_t brightness = value.toInt();
    LED->set_brightness(brightness);
    brightnessPublish.publish(brightness);
  });

  server.on("/effect", HTTP_POST, []() {
    String effect = server.arg("effect");
    String settings = server.arg("settings");
    String returnData = LED->parse_effect(effect, settings);
    server.send(200);
  });

  server.on("/sendStatus", HTTP_POST, []() {
    send_Current_Status_Mqtt();
    server.send(200);
  });

  server.on("/sendEffects", HTTP_POST, []() {
    String json = LED->currentPatterns();
    server.send(200, "text/json", json);
  });
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
  setupLamp();
  Serial.println("MQTT Connected!");
}

/** Callbacks for MQTT **/

void onoffcallback(char *data, uint16_t len)
{
  Serial.println(data);
  LED->set_power(data);
}

void brightnessCallback(char *data, uint16_t len)
{
  Serial.println(atoi(data));
  LED->set_brightness(atoi(data));
}

void nextEffectCallback(char *data, uint16_t len)
{
  const char nextEffectCommand[] = "nexteffect";
  if (strcmp(nextEffectCommand, (const char *)data) == 0)
  {
    LED->nextEffect();
    nextEffectPublish.publish("0");
  }
}

void previousEffectCallback(char *data, uint16_t len)
{
  const char previousEffectCommand[] = "previouseffect";
  if (strcmp(previousEffectCommand, (const char *)data) == 0)
  {
    LED->previousEffect();
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
  nextEffectSubscribe.setCallback(nextEffectCallback);

  mqtt.subscribe(&previousEffectSubscribe);
  previousEffectSubscribe.setCallback(previousEffectCallback);

  mqtt.subscribe(&statusSubscribe);
  statusSubscribe.setCallback(updateStatus);
}

void updateStatus(char *data, uint16_t len)
{
}

// send_Current_Status_Mqtt: Sends to Adafruit.io MQTT, the current status of the lamp in JSON
void send_Current_Status_Mqtt()
{
  String effectParameters;
  serializeJson(LED->effectParameters, effectParameters);
  String json = "{";
  json += "\"value\":{";
  json += "\"effectParameters\":" + effectParameters;
  json += ",\"currentPatternNumber\":" + String(LED->CurrentPatternNumber);
  json += "}";
  json += "}";
  Serial.print(json);
  statusPublish.publish(json.c_str());
}

//send_Current_Status_Http() - sends the current status through HTTP_GET
void send_Current_Status_Http()
{
  String effectParameters;
  serializeJson(LED->effectParameters, effectParameters);
  String json = "{";
  json += "\"power\":" + String(power) + ",";
  json += "\"brightness\":" + String(LED->brightness) + ",";
  json += "\"effect\":{";
  json += "\"effectParameters\":" + effectParameters;
  json += ",\"currentPatternNumber\":" + String(LED->CurrentPatternNumber);
  json += "}";
  json += "}";
  server.send(200, "text/json", json);
  json = String();
}