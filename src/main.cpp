#include <Arduino.h>
#include <FastLED.h>
#include "IntervalTimer.h"

#define NUM_LEDS 60
#define LED_PIN 4

CRGB leds[NUM_LEDS];

const unsigned long MOVE_INTERVAL = 100;
uint8_t pos = 0;
const unsigned long LOG_INTERVAL = 2000;
const unsigned long BLINK_INTERVAL = 500;
bool led_state = false;

IntervalTimer move_timer(MOVE_INTERVAL);
IntervalTimer log_timer(LOG_INTERVAL);
IntervalTimer blink_timer(BLINK_INTERVAL);

void setup() 
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  Serial.println(F("LED strip started"));
}

void loop() 
{
  if (blink_timer.ready())
  {
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
  }

  if (move_timer.ready()) 
  {
    fadeToBlackBy(leds, NUM_LEDS, 64);
    leds[pos] = CRGB::Blue;
    FastLED.show();
    pos++;

    if (pos >= NUM_LEDS)
    {
      pos = 0;
    }
  }

  if (log_timer.ready()) {
    Serial.print(F("Uptime: "));
    Serial.print(millis() / 1000);
    Serial.println(F(" s"));
  }
}