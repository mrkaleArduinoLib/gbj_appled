/*
  NAME:
  gbj_appled

  DESCRIPTION:
  Library manages an LED, usually BUILT-IN led for signalling purposes.
  - Library accepts different wiring of built-in led on various platforms.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_appled.git
*/
#ifndef GBJ_APPLED_H
#define GBJ_APPLED_H

#if defined(__AVR__)
  #include <Arduino.h>
  #include <inttypes.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <Arduino.h>
#endif
#include "gbj_timer.h"

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appled"

class gbj_appled
{
public:
  const String VERSION = "GBJ_APPLED 1.1.0";

  /*
    Constructor

    DESCRIPTION:
    Constructor creates the class instance object and sets operational
    parameters.

    PARAMETERS:
    pinLed - Number of GPIO pin of the microcontroller managing an led.
      - Data type: non-negative integer
      - Default value: LED_BUILTIN (GPIO depends on platform)
      - Limited range: 0 ~ 255
    reverse - Flag whether the pinLed works in reverse mode, i.e., active low.
      - Data type: boolean
      - Default value: true (preferrably for ESP8266, ESP32)
      - Limited range: true, false

    RETURN: object
  */
  inline gbj_appled(byte pinLed = LED_BUILTIN, bool reverse = true)
  {
    pin_ = pinLed;
    if (reverse)
    {
      ON = LOW;
      OFF = HIGH;
    }
    else
    {
      ON = HIGH;
      OFF = LOW;
    }
    timer_ = new gbj_timer(0);
  }

  /*
    Initialization

    DESCRIPTION:
    The method should be called in the SETUP section of a sketch and sets up
    the LED.

    PARAMETERS:
    enabled - Flag whether the LED is enabled in a sketch. Disabled LED is
      ignored entirely.
      - Data type: boolean
      - Default value: true
      - Limited range: true, false

    RETURN: Result code.
  */
  inline void begin(bool enabled = true)
  {
    pinMode(pin_, OUTPUT);
    enabled_ = true;
    off();
  }

  inline void enable()
  {
    enabled_ = true;
    timer_->restart();
  }
  inline void disable()
  {
    enabled_ = false;
    off();
  }
  inline void on()
  {
    if (isEnabled())
    {
      timer_->halt();
      digitalWrite(pin_, ON);
    }
    else
    {
      off();
    }
  }
  inline void off()
  {
    timer_->halt();
    digitalWrite(pin_, OFF);
  }
  inline void toggle()
  {
    if (isEnabled())
    {
      digitalWrite(pin_, digitalRead(pin_) ^ 1);
    }
    else
    {
      off();
    }
  }
  inline void blink() { blinkLed(Timing::PERIOD_NORMAL); }
  inline void blinkHurry() { blinkLed(Timing::PERIOD_HURRY); }
  inline void blinkFast() { blinkLed(Timing::PERIOD_FAST); }
  inline void blinkPattern(byte blinks = 3)
  {
    blinks_ = constrain(blinks, 2, 255);
    blinkHurry();
    counter_ = blinks_;
    patterned_ = true;
  }

  /*
    Processing.

    DESCRIPTION:
    The method should be called in an application sketch loop.
    It processes main functionality and is controlled by the internal timer.

    PARAMETERS: None

    RETURN: none
  */
  inline void run()
  {
    if (timer_->run())
    {
      if (isPatterned())
      {
        if (counter_)
        {
          if (isOn())
          {
            counter_--;
          }
          toggle();
        }
        else
        {
          if (halted_)
          {
            blinkPattern(blinks_);
          }
          else
          {
            digitalWrite(pin_, OFF);
            timer_->setPeriod(Timing::PERIOD_NORMAL);
            timer_->restart();
            halted_ = true;
          }
        }
      }
      else
      {
        toggle();
      }
    }
  }

  // Getters
  inline bool isOn() { return digitalRead(pin_) == ON; }
  inline bool isOff() { return digitalRead(pin_) == OFF; }
  inline bool isEnabled() { return enabled_; }
  inline bool isDisabled() { return !isEnabled(); }
  inline bool isBlinking() { return enabled_ && timer_->isActive(); }
  inline bool isPatterned() { return enabled_ && patterned_; }

private:
  enum Timing : unsigned int
  {
    PERIOD_NORMAL = 500,
    PERIOD_HURRY = 200,
    PERIOD_FAST = 100,
  };
  gbj_timer *timer_;
  byte ON, OFF;
  byte pin_, blinks_, counter_;
  bool enabled_, halted_, patterned_;

  inline void blinkLed(unsigned long period)
  {
    if (isEnabled())
    {
      digitalWrite(pin_, ON);
      halted_ = false;
      patterned_ = false;
      timer_->setPeriod(period);
      timer_->restart();
    }
    else
    {
      off();
    }
  }
};

#endif
