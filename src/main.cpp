#include <Arduino.h>
#include <FastLED.h>
#include "IntervalTimer.h"

constexpr uint8_t LED_PIN = 4;
constexpr uint8_t BUTTON_PIN = 2;
constexpr uint8_t SENSOR_PIN = 3;

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
const unsigned long WAIT_INTERVAL = 15000UL;
const unsigned long FADE_INTERVAL = 3000UL;

uint8_t pos = 0;
bool led_state = false;
uint8_t phase = 0;
uint8_t hue = 0;
uint8_t mode = 0;
bool last_btn_state = LOW;
bool btn_state;
bool last_sensor_state = LOW;
bool sensor_state;
uint8_t brightness_;
uint8_t last_brightness;
unsigned long elapsed;

IntervalTimer move_timer(MOVE_INTERVAL);
IntervalTimer log_timer(LOG_INTERVAL);
IntervalTimer blink_timer(BLINK_INTERVAL);
IntervalTimer breath_timer(BREATH_INTERVAL);
IntervalTimer rainbow_timer(RAINBOW_INTERVAL);
IntervalTimer wait_timer(WAIT_INTERVAL);
IntervalTimer fade_timer(FADE_INTERVAL);

enum State : uint8_t { STATE_OFF, STATE_ON, STATE_FADING };
State current_state = STATE_OFF;

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
      break;
  }

  FastLED.show();
}

void change_state(State next)
{
  switch (next)
  {
    case STATE_ON:
      wait_timer.reset();
      prepare_mode(mode);
      Serial.println(F("-> ON"));
      break;

    case STATE_FADING:
      fade_timer.reset();
      brightness_ = FastLED.getBrightness();
      Serial.println(F("-> FADING"));
      break;

    case STATE_OFF:
      FastLED.clear();
      FastLED.show();
      Serial.println(F("-> OFF"));
      break;
  }

  current_state = next;
}

void setup() 
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);
  FastLED.clear();
  FastLED.show();

  Serial.begin(9600);
  Serial.println(F("LED strip started"));
}

void loop() 
{
  btn_state = digitalRead(BUTTON_PIN);
  sensor_state = digitalRead(SENSOR_PIN);

  blink();
  log_uptime();

  switch(current_state)
  {
    case STATE_OFF:

      if(sensor_state)
      {
        change_state(STATE_ON);
      }

      break;

    case STATE_ON:

      if(sensor_state)
      {
        wait_timer.reset();

        if(!last_sensor_state)
        {
          Serial.println(F("ON time added"));
        }
      }

      if (wait_timer.expired())
      {
        Serial.println(F("timeout"));

        change_state(STATE_FADING);
      }

      // Reading button for mode change while in ON state
      if(btn_state && !last_btn_state)
      {
        mode = (mode + 1) % MODE_NUM;
        prepare_mode(mode);
        Serial.print(F("Mode: ")); 
        Serial.println(mode);
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

      break;

    case STATE_FADING:
      

      if(sensor_state)
      {
        change_state(STATE_ON);
        break;
      }

      if(fade_timer.expired())
      {
        change_state(STATE_OFF);
        break;
      }

      elapsed = millis() - fade_timer.get_last();
      last_brightness = brightness_ * (FADE_INTERVAL - elapsed) / FADE_INTERVAL;
      FastLED.setBrightness(last_brightness);
      FastLED.show();

      break;
  }

  last_btn_state = btn_state;
  last_sensor_state = sensor_state;
}