#include "led_effects.h"

Led_effects::Led_effects(uint8_t init_brightness)
{

  CurrentPatternNumber = 1;
  brightness = init_brightness;
  oldBrightness = init_brightness;
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(brightness);
  fill_solid(leds, NUM_LEDS, CRGB(244, 244, 244));
  FastLED.show();
};

//Returns a serialized JSON String with all Effects with their available options
String Led_effects::currentPatterns()
{
}

void Led_effects::update_leds()
{ //If its 0 or one its in solid color mode or custom color mode
  PatternList Patterns[] = {&Led_effects::nullPattern, &Led_effects::juggle, &Led_effects::sinelon};
  if (CurrentPatternNumber != 0 || CurrentPatternNumber != 1)
  {
    (this->*Patterns[CurrentPatternNumber])();
  }
}

String Led_effects::parse_effect(String new_effect, String parameters)
{

  //Deserialize the JSON
  DeserializationError error = deserializeJson(ledJSON, parameters);
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
    uint8_t r = ledJSON["r"];
    uint8_t g = ledJSON["g"];
    uint8_t b = ledJSON["b"];
    set_solid(r, g, b);
    CurrentPatternNumber = 0;
  }
  if (new_effect == "singleColor")
  {
    //Grab the colors from the Parameters JSON
    uint8_t led = ledJSON["led"];
    uint8_t r = ledJSON["r"];
    uint8_t g = ledJSON["g"];
    uint8_t b = ledJSON["b"];
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

void Led_effects::set_brightness(uint8_t newBrightness)
{
  Serial.println(newBrightness);
  int value = map(newBrightness, 0, 100, 0, 255); //Map it to full brightness
  if (value > 255)
    value = 255;
  else if (value < 0)
    value = 0;
  brightness = value;
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