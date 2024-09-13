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
    is suitable at ESP8266-01, where built-in led is connected to serial TX pin,
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
    status_.pin = pinLed;
    status_.blocked = block;
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
      pinMode(status_.pin, OUTPUT);
    }
    on();
    enabled ? enable() : disable();
  }

  inline void block() { status_.blocked = true; }
  inline void free() { status_.blocked = false; }
  inline void enable()
  {
    status_.enabled = true;
    execMode();
  }
  inline void disable()
  {
    status_.enabled = false;
    off();
  }
  inline void on()
  {
    setMode(Modus::MODE_ON);
    timer_->halt();
    status_.lit = true;
    if (isEnabled() && isFree())
    {
      digitalWrite(status_.pin, ON);
    }
  }
  inline void off()
  {
    timer_->halt();
    status_.lit = false;
    if (isEnabled())
    {
      setMode(Modus::MODE_OFF);
    }
    if (isFree())
    {
      digitalWrite(status_.pin, OFF);
    }
  }
  inline void toggle()
  {
    status_.lit = !status_.lit;
    if (isEnabled() && isFree())
    {
      digitalWrite(status_.pin, digitalRead(status_.pin) ^ 1);
    }
  }
  inline void blink()
  {
    setMode(Modus::MODE_BLINK);
    blinkLed(Timing::PERIOD_NORMAL);
  }
  inline void blinkHurry()
  {
    setMode(Modus::MODE_HURRY);
    blinkLed(Timing::PERIOD_HURRY);
  }
  inline void blinkFast()
  {
    setMode(Modus::MODE_FAST);
    blinkLed(Timing::PERIOD_FAST);
  }
  inline void blinkPattern(byte blinks = 3)
  {
    setMode(Modus::MODE_PATTERN);
    setBlinks(constrain(blinks, 2, 255));
    status_.counter = status_.blinks;
    blinkLed(Timing::PERIOD_HURRY);
  }
  inline void restoreMode()
  {
    if (isEnabled() && isFree())
    {
      setMode(status_.modeOld);
      if (getMode() == Modus::MODE_PATTERN)
      {
        byte blinks = status_.blinks;
        status_.blinks = status_.blinksOld;
        status_.blinksOld = blinks;
      }
      execMode();
    }
  }
  inline void forceLit()
  {
    if (getMode() != Modus::MODE_OFF)
    {
      status_.lit = true;
      if (isEnabled() && isFree())
      {
        digitalWrite(status_.pin, ON);
      }
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
    if (timer_->run())
    {
      if (isPatterned())
      {
        if (status_.counter > 0)
        {
          if (isLit())
          {
            status_.counter--;
          }
          toggle();
        }
        else
        {
          // Restart patterned blinking
          if (status_.halted)
          {
            setMode(Modus::MODE_PATTERN);
            status_.counter = status_.blinks;
            blinkLed(Timing::PERIOD_HURRY);
          }
          // Halt patterned blinking
          else
          {
            timer_->setPeriod(Timing::PERIOD_NORMAL);
            timer_->restart();
            status_.halted = true;
            status_.lit = false;
            if (isEnabled() && isFree())
            {
              digitalWrite(status_.pin, OFF);
            }
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
  inline bool isBlocked() { return status_.blocked; }
  inline bool isFree() { return !isBlocked(); }
  inline bool isEnabled() { return status_.enabled; }
  inline bool isDisabled() { return !isEnabled(); }
  inline bool isLit() { return status_.lit; }
  inline bool isDim() { return !isLit(); }
  inline bool isOff() { return status_.mode == Modus::MODE_OFF; }
  inline bool isOn() { return status_.mode == Modus::MODE_ON; }
  inline bool isPatterned() { return status_.mode == Modus::MODE_PATTERN; }
  inline bool isBlinking() { return timer_->isActive(); }
  inline String getStatusOn() { return "ON"; }
  inline String getStatusOff() { return "OFF"; }
  inline String getStatus()
  {
    return isEnabled() ? getStatusOn() : getStatusOff();
  }
  inline byte getMode() { return static_cast<byte>(status_.mode); }
  inline String getModeText()
  {
    switch (status_.mode)
    {
      case Modus::MODE_ON:
        return "on";
        break;

      case Modus::MODE_BLINK:
        return "blink";
        break;

      case Modus::MODE_HURRY:
        return "hurry";
        break;

      case Modus::MODE_FAST:
        return "fast";
        break;

      case Modus::MODE_PATTERN:
        return "pattern";
        break;

      default:
        return "off";
        break;
    }
  }
  inline byte getBlinks() { return status_.blinks; }

  // Setters
  inline void setAbility(bool enabled) { enabled ? enable() : disable(); }

private:
  enum Timing : unsigned int
  {
    PERIOD_NORMAL = 500,
    PERIOD_HURRY = 200,
    PERIOD_FAST = 100,
  };
  enum Modus : byte
  {
    MODE_OFF,
    MODE_ON,
    MODE_BLINK,
    MODE_HURRY,
    MODE_FAST,
    MODE_PATTERN,
  };
  struct Status
  {
    byte pin;
    byte counter;
    bool blocked;
    bool enabled;
    bool halted;
    bool lit;
    byte blinks;
    byte blinksOld;
    Modus mode;
    Modus modeOld;
  } status_;
  gbj_timer *timer_;
  byte ON, OFF;

  inline void blinkLed(unsigned long period)
  {
    timer_->setPeriod(period);
    timer_->restart();
    status_.halted = false;
    status_.lit = true;
    if (isEnabled() && isFree())
    {
      digitalWrite(status_.pin, ON);
    }
  }
  inline void execMode()
  {
    switch (status_.mode)
    {
      case Modus::MODE_OFF:
        off();
        break;

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
        blinkPattern(status_.blinks);
        break;

      default:
        off();
        break;
    }
  }
  inline void setMode(Modus mode)
  {
    if (mode != status_.mode || status_.blinks != status_.blinksOld)
    {
      status_.modeOld = status_.mode;
      status_.mode = mode;
    }
  }
  inline void setBlinks(byte blinks)
  {
    if (blinks != status_.blinks)
    {
      status_.blinksOld = status_.blinks;
      status_.blinks = blinks;
    }
  }
};

#endif
