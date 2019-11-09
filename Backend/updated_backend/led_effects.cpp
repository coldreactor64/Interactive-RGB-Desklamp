#include "led_effects.h"

PatternList Patterns[] = {&Led_effects::nullPattern, &Led_effects::juggle, &Led_effects::sinelon};

Led_effects::Led_effects(uint8_t init_brightness)
{
  lengthPatterns = 2; //Set the length of Patterns

  CurrentPatternNumber = 1; //Default pattern at startup

  brightness = init_brightness; //Set default brightnesses at startup
  oldBrightness = init_brightness;

  /*startup LEDS and fill with solid color*/
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(brightness);
  fill_solid(leds, NUM_LEDS, CRGB(244, 244, 244));
  FastLED.show();
};

//Current patterns parameter options are currently [color,led]
//Color is RGB value seperated in JSON {r:0,g:0,b:0}
//led is the led number for the effect indexed from 0 {led:1}
String Led_effects::currentPatterns()
{

  String singleSelectJSON = "{";
  singleSelectJSON += "\"name\": \"Single Select\",";
  singleSelectJSON += "\"parameters\": [\"color\", \"led\"]";
  singleSelectJSON += "}";

  String solidColorJSON = "{";
  solidColorJSON += "\"name\": \"Solid Color\",";
  solidColorJSON += "\"parameters\": [\"color\"]";
  solidColorJSON += "}";

  String effectsJSON = "{";
  effectsJSON += "\"effects\":{";
  effectsJSON += "\"effect1\":" + solidColorJSON + ",";
  effectsJSON += "\"effect2\":" + singleSelectJSON;
  effectsJSON += "}";
  effectsJSON += "}";

  return effectsJSON;
}

void Led_effects::update_leds()
{ //If its 0 or one its in solid color mode or custom color mode

  if (CurrentPatternNumber != 0 || CurrentPatternNumber != 1)
  {
    (this->*Patterns[CurrentPatternNumber])();
  }
}

String Led_effects::parse_effect(String new_effect, String parameters)
{

  //Deserialize the JSON Parameters from the String
  DeserializationError error = deserializeJson(effectParameters, parameters);
  // Test if parsing succeeds.
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "fail";
  }

  if (new_effect == "solidColor")
  {
    //Grab the colors from the Parameters JSON
    uint8_t r = effectParameters["r"];
    uint8_t g = effectParameters["g"];
    uint8_t b = effectParameters["b"];
    set_solid(r, g, b);
    CurrentPatternNumber = 0;
  }
  if (new_effect == "singleColor")
  {
    //Grab the colors from the Parameters JSON
    uint8_t led = effectParameters["led"];
    uint8_t r = effectParameters["r"];
    uint8_t g = effectParameters["g"];
    uint8_t b = effectParameters["b"];
    set_single_color(led, r, g, b);
    CurrentPatternNumber = 0;
  }
  if (new_effect == "juggle")
  {
    CurrentPatternNumber = 1;
    Serial.println('juggle');
  }
  if (new_effect == "sinleon")
  {
    CurrentPatternNumber = 2;
    Serial.println('sinleon');
  }
  else
  {
    return String("No");
  }
  return String("Yes");
}

void Led_effects::set_power(char *command)
{
  const char onCommand[] = "on";
  const char offCommand[] = "off";
  Serial.println(command);
  if (strcmp(onCommand, (const char *)command) == 0)
  {
    Serial.println("On");
    set_brightness(oldBrightness);
  }
  if (strcmp(offCommand, (const char *)command) == 0)
  {
    //Make the old brightness stored in case we want to turn it back on
    Serial.println("Off");
    oldBrightness = brightness;
    set_brightness(0);
  }
}

//set_brightness: sets brightness on input of a uint8_t from 0-100
void Led_effects::set_brightness(uint8_t newBrightness)
{
  int value = map(newBrightness, 0, 100, 0, 255); //Map it to full brightness
  if (value > 255)
    value = 255;
  else if (value < 0)
    value = 0;
  /* Put this as new brightness instead of value because 
   * value is the actual value needed to go to FastLed.setBrightness
   * and when we do an operation we want to be relative to 0-100
   * aka when we turn on and off the lamp using set_power, we want to use this function
   * and we will overflow if we input the actual brightness value instead of
   * the 0-100 brightness value
  */
  brightness = newBrightness;
  FastLED.setBrightness(value);
};

void Led_effects::set_single_color(uint8_t LED, uint8_t r, uint8_t g, uint8_t b)
{
  leds[LED] = CRGB(r, g, b);
};

void Led_effects::set_solid(uint8_t r, uint8_t g, uint8_t b)
{
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
};

void Led_effects::juggle()
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

void Led_effects::sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(0, 255, 192);
}

void Led_effects::nullPattern()
{
}

void Led_effects::nextEffect()
{
  if (CurrentPatternNumber == 0)
  {
    CurrentPatternNumber = 1;
  }
  else if (CurrentPatternNumber == lengthPatterns)
  {
    CurrentPatternNumber = 1;
  }
  else
  {
    CurrentPatternNumber++;
  }
}

void Led_effects::previousEffect()
{
  if (CurrentPatternNumber - 1 == 0)
  {
    CurrentPatternNumber = lengthPatterns;
  }
  else if (CurrentPatternNumber == 0)
  {
    CurrentPatternNumber = lengthPatterns;
  }
  else
  {
    CurrentPatternNumber--;
  }
}