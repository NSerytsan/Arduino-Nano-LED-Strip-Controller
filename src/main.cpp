#include <Arduino.h>
#include <FastLED.h>
#include "IntervalTimer.h"

constexpr uint8_t LED_PIN = 4;
constexpr uint16_t NUM_LEDS = 60;
constexpr uint8_t BRIGHTNESS = 50;
constexpr uint8_t TAIL_FADE = 64;
constexpr uint8_t BREATH_MIN = 5;
constexpr uint8_t MODE_NUM = 3;

CRGB leds[NUM_LEDS];

const unsigned long MOVE_INTERVAL = 100;
const unsigned long LOG_INTERVAL = 2000;
const unsigned long BLINK_INTERVAL = 500;
const unsigned long BREATH_INTERVAL = 20;
const unsigned long RAINBOW_INTERVAL = 20;
const unsigned long MODE_INTERVAL = 10000;


uint8_t pos = 0;
bool led_state = false;
uint8_t phase = 0;
uint8_t hue = 0;
uint8_t mode = 0;

IntervalTimer move_timer(MOVE_INTERVAL);
IntervalTimer log_timer(LOG_INTERVAL);
IntervalTimer blink_timer(BLINK_INTERVAL);
IntervalTimer breath_timer(BREATH_INTERVAL);
IntervalTimer rainbow_timer(RAINBOW_INTERVAL);
IntervalTimer mode_timer(MODE_INTERVAL);

void blink()
{
  if (blink_timer.ready())
  {
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
  }
}

void breath()
{
  if(breath_timer.ready())
  {
    phase++;
    FastLED.setBrightness(map(sin8(phase), 0, 255, BREATH_MIN, BRIGHTNESS));
    FastLED.show();
  }
}
void rainbow()
{
  if(rainbow_timer.ready())
  {
    hue++;
    fill_rainbow(leds, NUM_LEDS, hue, 7);
    FastLED.show();
  }
}

void moveLED()
{
  if (move_timer.ready()) 
  {
    fadeToBlackBy(leds, NUM_LEDS, TAIL_FADE);
    leds[pos] = CRGB::Blue;
    FastLED.show();
    
    pos = (pos + 1) % NUM_LEDS;
  }
}
void log_uptime()
{
  if (log_timer.ready()) {
    Serial.print(F("Uptime: "));
    Serial.print(millis() / 1000);
    Serial.println(F(" s"));
  }
}

void prepare_mode(uint8_t m)
{
  FastLED.clear();
  FastLED.setBrightness(BRIGHTNESS);

  switch (m)
  {
    case 0:
      fill_solid(leds, NUM_LEDS, CRGB::Amethyst);
      break;
  
    case 1:
      pos = 0;
  }

  FastLED.show();
}

void setup() 
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  prepare_mode(mode);
  Serial.println(F("LED strip started"));
}

void loop() 
{
  blink();
  log_uptime();

  if(mode_timer.ready())
  {
    mode = (mode + 1) % MODE_NUM;
    prepare_mode(mode);
  }

  switch (mode)
  {
    case 0:
      breath();
      break;
  
    case 1:
      moveLED();
      break;
  
    case 2:
      rainbow();
      break;
  }
  
}