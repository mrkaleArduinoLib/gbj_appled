/*
  NAME:
  gbj_appled

  DESCRIPTION:
  Library manages an LED, usually BUILT-IN led for signalling purposes.
  - Library accepts different wiring of a built-in led on various platforms.

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
  const String VERSION = "GBJ_APPLED 1.3.0";

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
    reverse - Flag whether the led works in reverse mode, i.e., active low.
      - Data type: boolean
      - Default value: true (preferrably for ESP8266, ESP32)
      - Limited range: true, false
    block - Flag whether the GPIO pin for led is not controlled alltogether. It
    is suitable at ESP8266-01, where builtin led is connected to serial TX pin,
    so that the using led and serial monitor at once is not possible.
      - Data type: boolean
      - Default value: false
      - Limited range: true, false

    RETURN: object
  */
  inline gbj_appled(byte pinLed = LED_BUILTIN,
                    bool reverse = true,
                    bool block = false)
  {
    pin_ = pinLed;
    blocked_ = block;
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

    RETURN: None
  */
  inline void begin(bool enabled = true)
  {
    if (isFree())
    {
      pinMode(pin_, OUTPUT);
    }
    enabled ? enable() : disable();
    on();
  }

  inline void block() { blocked_ = true; }
  inline void free() { blocked_ = false; }
  inline void enable()
  {
    enabled_ = true;
    switch (mode_)
    {
      case Modus::MODE_ON:
        on();
        break;

      case Modus::MODE_BLINK:
        blink();
        break;

      case Modus::MODE_HURRY:
        blinkHurry();
        break;

      case Modus::MODE_FAST:
        blinkFast();
        break;

      case Modus::MODE_PATTERN:
        blinkPattern(blinks_);
        break;

      default:
        off();
        break;
    }
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
      if (isFree())
      {
        timer_->halt();
        digitalWrite(pin_, ON);
      }
      mode_ = Modus::MODE_ON;
    }
    else
    {
      off();
    }
  }
  inline void off()
  {
    if (isFree())
    {
      timer_->halt();
      digitalWrite(pin_, OFF);
    }
  }
  inline void toggle()
  {
    if (isEnabled())
    {
      if (isFree())
      {
        digitalWrite(pin_, digitalRead(pin_) ^ 1);
      }
    }
    else
    {
      off();
    }
  }
  inline void blink()
  {
    blinkLed(Timing::PERIOD_NORMAL);
    mode_ = Modus::MODE_BLINK;
  }
  inline void blinkHurry()
  {
    blinkLed(Timing::PERIOD_HURRY);
    mode_ = Modus::MODE_HURRY;
  }
  inline void blinkFast()
  {
    blinkLed(Timing::PERIOD_FAST);
    mode_ = Modus::MODE_FAST;
  }
  inline void blinkPattern(byte blinks = 3)
  {
    blinks_ = constrain(blinks, 2, 255);
    if (!isPatterned())
    {
      blinkPatternRestart();
    }
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
    if (isBlocked())
    {
      return;
    }
    if (timer_->run())
    {
      if (isPatterned())
      {
        if (counter_)
        {
          if (isLit())
          {
            counter_--;
          }
          toggle();
        }
        else
        {
          if (halted_)
          {
            blinkPatternRestart();
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
  inline bool isBlocked() { return blocked_; }
  inline bool isFree() { return !isBlocked(); }
  inline bool isEnabled() { return enabled_; }
  inline bool isDisabled() { return !isEnabled(); }
  inline bool isLit() { return isBlocked() ? false : digitalRead(pin_) == ON; }
  inline bool isDim() { return isBlocked() ? false : digitalRead(pin_) == OFF; }
  inline bool isOff() { return isDim() && !isBlinking(); }
  inline bool isOn()
  {
    return isBlocked() ? false : isEnabled() && mode_ == Modus::MODE_ON;
  }
  inline bool isBlinking()
  {
    return isBlocked() ? false : isEnabled() && timer_->isActive();
  }
  inline bool isPatterned()
  {
    return isBlocked() ? false : isEnabled() && mode_ == Modus::MODE_PATTERN;
  }

private:
  enum Timing : unsigned int
  {
    PERIOD_NORMAL = 500,
    PERIOD_HURRY = 200,
    PERIOD_FAST = 100,
  };
  enum Modus
  {
    MODE_OFF,
    MODE_ON,
    MODE_BLINK,
    MODE_HURRY,
    MODE_FAST,
    MODE_PATTERN,
  };
  gbj_timer *timer_;
  Modus mode_;
  byte ON, OFF;
  byte pin_, blinks_, counter_;
  bool blocked_, enabled_, halted_;

  inline void blinkLed(unsigned long period)
  {
    if (isEnabled())
    {
      digitalWrite(pin_, ON);
      timer_->setPeriod(period);
      timer_->restart();
      halted_ = false;
    }
    else
    {
      off();
    }
  }
  inline void blinkPatternRestart()
  {
    blinkHurry();
    mode_ = Modus::MODE_PATTERN;
    counter_ = blinks_;
  }
};

#endif
