#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "FastLED.h"
#include "ArduinoJson.h"
/**RGB LED Configuration**/
#define DATA_PIN 4           //ESP8266 Pin Number
#define LED_TYPE WS2812      //LED Type
#define COLOR_ORDER GRB      //Order of Color on RGB Strip
#define NUM_LEDS 29          //Number of LEDs in strip
#define MILLI_AMPS 2000      //Set Maximum Current of the LEDs
#define FRAMES_PER_SECOND 30 // FPS (default: 120)


class Led_effects
{

public:
  Led_effects(uint8_t);
  void set_power(char*);
  void set_brightness(uint8_t);
  void update_leds();
  void nextEffect();
  void previousEffect();

  String parse_effect(String, String);
  CRGB leds[NUM_LEDS];
  StaticJsonDocument<300> effectParameters;
  uint8_t brightness;
  uint8_t oldBrightness;
  uint8_t CurrentPatternNumber;
  uint8_t lengthPatterns;
  String currentPatterns();
  void juggle();
  void sinelon();
  void nullPattern();
private:
  void init_leds(int);
  void set_single_color(uint8_t, uint8_t, uint8_t, uint8_t);
  void set_solid(uint8_t, uint8_t, uint8_t);
};

typedef void (Led_effects::*PatternList)();