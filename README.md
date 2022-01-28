<a id="library"></a>

# gbj\_appled
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of a `signal LED`, which is usually built-in LED of a microcontroller development board. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concern`.
* The library is reusable for various projects without need to code a led management.
* Update in library is valid for all involved projects.


## Fundamental functionality

* The library controls visibility (on, off) an led always in the same way regardless the led is active high (arduino) or active low (ESP8266, ESP32).
* It controls ability (enabled, disabled) of the led.
* It provides time period for normal and fast blinking.


<a id="internals"></a>

## Internal parameters
Internal parameters are hard-coded in the library usually as enumerations and have no setters associated.

* **Time period for normal blinking** (500 ms): This parameter for 1 blink per second (1 Hz frequency) is suitable for signalling normal operation of a microcontroller. Its getter is [getPeriodNormal()](#period).
* **Time period for fast blinking** (100 ms): This parameter for 5 blinks per second (5 Hz frequency) is suitable for signalling abnormal, usually erroneous operation of a microcontroller. Its getter is [getPeriodFast()](#period).


<a id="dependency"></a>

## Dependency

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif ESP8266 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.


<a id="constants"></a>

## Constants

* **gbj\_appled::VERSION**: Name and semantic version of the library.


<a id="interface"></a>

## Interface
* [gbj_appled()](#gbj_appled)
* [begin()](#begin)
* [enable()](#allow)
* [disable()](#allow)
* [on()](#switch)
* [off()](#switch)
* [toggle()](#switch)

##### Getters
* [isOn()](#state)
* [isOff()](#state)
* [isEnabled()](#ability)
* [isDisabled()](#ability)
* [getPeriodNormal()](#period)
* [getPeriodFast()](#period)


<a id="gbj_appled"></a>

## gbj_appled()

#### Description
Constructor creates the class instance object and initiates state of the led, especially values for turned on or off state depending on reversebility of the led.

#### Syntax
    gbj_appled(byte pinLed, bool reverse)

#### Parameters
* **pinLed**: Number of a GPIO pin, where the signalling led is connected. Usually it is a built-in led or an auxilliary led (NodeMCU).
  * *Valid values*: available output GPIO pins
  * *Default value*: LED_BUILTIN (depends on platform)


* **reverse**: Flag determining reversibility of the led, i.e., whether the led works in reverse mode (active low) or in direct mode (active high).
  * *Valid values*: true or false
  * *Default value*: true (for ESP8266, ESP)

#### Returns
Object performing led management.

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The initialization method of the instance object, which should be called in the setup section of a sketch.
- The method activates an led pin and sets ability of it.
- The ability can be change later with corresponding method.

#### Syntax
    void begin(bool enabled)

#### Parameters
* **enabled**: Flag defining the ability of an LED. If disabled, the led is ignored entirely and never lits.
  * *Valid values*: true or false
  * *Default value*: true

#### Returns
None

#### See also
[enable(), disable()](#allow)

[Back to interface](#interface)


<a id="allow"></a>

## enable(), disable()

#### Description
The methods manipulate ability of the led, either enables or disable it.

#### Syntax
    void enable()
    void disable()

#### Parameters
None

#### Returns
None

#### See also
[begin()](#begin)

[Back to interface](#interface)


<a id="switch"></a>

## on(), off(), toggle()

#### Description
The particular method sets corresponding visibility of the led, either switch it on, off, or changes its state.

#### Syntax
    void on()
    void off()
    void toggle()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="state"></a>

## isOn(), isOff()

#### Description
The particular getter returns flag determining whether corresponding visibility of the led is valid.

#### Syntax
    bool isOn()
    bool isOff()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding led's visibility.

#### See also
[on(), off(), toggle()](#switch)

[Back to interface](#interface)


<a id="ability"></a>

## isEnabled(), isDisabled()

#### Description
The particular getter returns flag determining whether corresponding ability of the led is valid.

#### Syntax
    bool isEnabled()
    bool isDisabled()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding led's ability.

#### See also
[enable(), disable()](#allow)

[Back to interface](#interface)


<a id="period"></a>

## getPeriodNormal(), getPeriodFast()

#### Description
The particular getter returns period in milliseconds for normal or fast blinking of the led. It is the parameter for a timer controlling the visibility of the led.

#### Syntax
    unsigned int getPeriodNormal()
    unsigned int getPeriodFast()

#### Parameters
None

#### Returns
Positive integer time period for related time period controlling the led. In fact, it is a time period for visibility states of the led.

#### Example
```cpp
gbj_appled led = gbj_appled(PIN_LED);
void ledToggle()
{
  led.toggle();
}
gbj_timer timerLed(led.getPeriodNormal(), ledToggle);
void setup()
{
    led.begin();
}
void loop
{
    timerLed.run();
}
```

#### See also
[Internal parameters](#internals)

[Back to interface](#interface)
