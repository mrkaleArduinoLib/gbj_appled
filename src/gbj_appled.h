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
  static const String VERSION;

  /*
    Constructor

    DESCRIPTION:
    Constructor creates the class instance object and sets operational
    parameters.

    PARAMETERS:
    pinLed - Number of GPIO pin of the microcontroller managing an led.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 255

    RETURN: object
  */
  inline gbj_appled(byte pinLed = LED_BUILTIN, bool reverse = false)
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

  inline void begin(bool initOn = true, bool enabled = true)
  {
    enabled_ = enabled;
    pinMode(pin_, OUTPUT);
    initOn ? on() : off();
  }

  void enable()
  {
    enabled_ = true;
    on();
  }
  void disable()
  {
    enabled_ = false;
    off();
  }
  void on()
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
  void off() { digitalWrite(pin_, OFF); }
  void toggle()
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
  bool isOn() { return digitalRead(pin_) == ON; }
  bool isOff() { return digitalRead(pin_) == OFF; }
  bool isEnabled() { return enabled_; }
  bool isDisabled() { return !isEnabled(); }

private:
  byte pin_;
  byte ON, OFF;
  bool enabled_;
};

#endif
