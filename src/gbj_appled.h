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

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appled"

class gbj_appled
{
public:
  const String VERSION = "GBJ_APPLED 1.0.1";

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
    enabled_ = enabled;
    pinMode(pin_, OUTPUT);
    off();
  }

  inline void enable()
  {
    enabled_ = true;
    on();
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
      digitalWrite(pin_, ON);
    }
    else
    {
      off();
    }
  }
  inline void off() { digitalWrite(pin_, OFF); }
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

  // Getters
  inline bool isOn() { return digitalRead(pin_) == ON; }
  inline bool isOff() { return digitalRead(pin_) == OFF; }
  inline bool isEnabled() { return enabled_; }
  inline bool isDisabled() { return !isEnabled(); }
  inline unsigned int getPeriodNormal() { return Timing::PERIOD_NORMAL;}
  inline unsigned int getPeriodFast() { return Timing::PERIOD_FAST;}

private:
  enum Timing : unsigned int
  {
    PERIOD_NORMAL = 500,
    PERIOD_FAST = 100,
  };
  byte ON, OFF;
  byte pin_;
  bool enabled_;
};

#endif
