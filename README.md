# gbj\_appled
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of a `signal LED`, which is usually built-in LED of a microcontroller development board. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concerns`.
* The library is reusable for various projects without need to code a led management.
* Update in library is valid for all involved projects.


## Fundamental functionality
* The library controls visibility (on, off) an led always in the same way regardless the led is active high (arduino, ESP32) or active low (ESP8266).
* It controls accessibility of an led's GPIO, i.e., can totally avoid manipulation with the corresponding pin. It is neccessary for microcontroller, where builtin led is connected to serial TX pin, so that using the led and serial monitor at once is not possible.
* It controls accessibility (blocked, free) of the led.
* It controls ability (enabled, disabled) of the led.
* It controls blinking (steady, patterned) of the led.
* The library utilizes internal timer for blinking.


<a id="internals"></a>

## Internal parameters
Internal parameters are hard-coded in the library usually as enumerations and have no setters and getters associated.

* **Time period for normal blinking** (`500 ms`): This parameter for 1 blink per second (1 Hz frequency) is suitable for signalling normal operation of a microcontroller.
* **Time period for hurry blinking** (`200 ms`): This parameter for 5 blinks per 2 second (2.5 Hz frequency) is utilized for patterned blinking and suitable for signalling specific errors by number of blinks in a burst.
* **Time period for fast blinking** (`100 ms`): This parameter for 5 blinks per second (5 Hz frequency) is suitable for signalling abnormal, usually general erroneous operation of a microcontroller.


<a id="dependency"></a>

## Dependency
* **gbj\_timer**: Library for executing internal timer within an instance object loaded from the file `gbj_timer.h`.

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif ESP8266 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
* **Arduino.h**: Main include file for the Arduino platform.


<a id="interface"></a>

## Interface
* [gbj_appled()](#gbj_appled)
* [begin()](#begin)
* [run()](#run)
* [block()](#ignore)
* [free()](#ignore)
* [enable()](#allow)
* [disable()](#allow)
* [on()](#switch)
* [off()](#switch)
* [toggle()](#switch)
* [blink()](#blink)
* [blinkHurry()](#blink)
* [blinkFast()](#blink)
* [blinkPattern()](#pattern)
* [forceLit()](#forceLit)

### Getters
* [isBlocked()](#access)
* [isFree()](#access)
* [isEnabled()](#ability)
* [isDisabled()](#ability)
* [isLit()](#state)
* [isDim()](#state)
* [isOn()](#visibility)
* [isOff()](#visibility)
* [isBlinking()](#blinking)
* [isPatterned()](#blinking)
* [getStatusOn()](#getStatusFlags)
* [getStatusOff()](#getStatusFlags)
* [getStatus()](#getStatus)
* [getMode()](#getModus)
* [getModeText()](#getModus)
* [getBlinks()](#getBlinks)

### Setters
* [setAbility()](#setAbility)


<a id="gbj_appled"></a>

## gbj_appled()

#### Description
Constructor creates the class instance object and initiates state of the led, especially values for turned on or off state depending on reversebility of the led.

#### Syntax
    gbj_appled(byte pinLed, bool reverse, bool block)

#### Parameters
* **pinLed**: Number of a GPIO pin, where the signalling led is connected. Usually it is a built-in led or an auxilliary led (NodeMCU).
  * *Valid values*: available output GPIO pins
  * *Default value*: LED_BUILTIN (depends on platform)


* **reverse**: Flag determining polarity of the led, i.e., whether the led works in reverse mode (active low) or in direct mode (active high).
  * *Valid values*: true or false
  * *Default value*: true (preferrably for ESP8266, ESP)


* **block**: Flag whether the GPIO pin for led is not controlled alltogether. It is suitable at ESP8266-01, where builtin led is connected to serial TX pin, so that the using led and serial monitor at once is not possible.
  * *Valid values*: true or false
  * *Default value*: false

#### Returns
Object performing led management.

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The initialization method of the instance object, which should be called in the setup section of a sketch.
* The method activates an led pin, sets ability of it, and turns it on, if it is enabled.
* The ability can be change later with corresponding method.

#### Syntax
    void begin(bool enabled)

#### Parameters
* **enabled**: Flag defining the ability of an LED. If disabled, the led is ignored entirely and never lits, but internal code is running. At running it can be changed by the setter [setAbility()](#setAbility).
  * *Valid values*: true or false
  * *Default value*: true

#### Returns
None

#### See also
[enable(), disable()](#allow)

[setAbility()](#setAbility)

[Back to interface](#interface)


<a id="run"></a>

## run()

#### Description
The method executes an led blinking and should be called frequently, usually in the loop function of a sketch.

#### Syntax
    void run()

#### Parameters
None

#### Returns
None

#### See also
[blink(), blinkHurry(), blinkFast()](#blink)

[blinkPattern()](#pattern)

[Back to interface](#interface)


<a id="ignore"></a>

## block(), free()

#### Description
The methods manipulate accessibility of the led's GPIO pin, either blocks or frees it.
* Blocking the led is useful when a led's GPIO pin should not be allocated, e.g, when it is built-in one and is used for serial communication.
* After changing the accessibility of a led's GPIO pin during the firmware run, its mode should be set exactly in a sketch accordingly to the new usage of it.
* When the led is blocked, it is turned off even if the led is enabled.

#### Syntax
    void block()
    void free()

#### Parameters
None

#### Returns
None

#### See also
[isBlocked(), isFree()](#access)

[enable(), disable()](#allow)

[Back to interface](#interface)


<a id="allow"></a>

## enable(), disable()

#### Description
The methods manipulate ability of the led, either enables or disables it.
* If the led is disabled, the internal code is working the same way as in enabled mode, but the physical manipulation with led's GPIO pin is suppressed as it was at blocked led.

#### Syntax
    void enable()
    void disable()

#### Parameters
None

#### Returns
None

#### See also
[isEnabled(), isDisabled()](#ability)

[setAbility()](#setAbility)

[block(), free()](#ignore)

[begin()](#begin)

[Back to interface](#interface)


<a id="setAbility"></a>

## setAbility()

#### Description
The method sets ability of the led by the value of input argument. It is a setter for the input argument of the method [begin](#begin).

#### Syntax
    void setAbility(bool enabled)

#### Parameters
* **enabled**: Flag defining the ability of an LED. If disabled, the led is ignored entirely and never lits.
  * *Valid values*: true or false
  * *Default value*: none

#### Returns
None

#### See also
[isEnabled(), isDisabled()](#ability)

[begin()](#begin)

[Back to interface](#interface)


<a id="switch"></a>

## on(), off(), toggle()

#### Description
The particular method sets corresponding steady visibility of the led if it is enabled, either switch it on, off, or changes its state.

#### Syntax
    void on()
    void off()
    void toggle()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="blink"></a>

## blink(), blinkHurry(), blinkFast()

#### Description
The particular method starts corresponding blinking mode of an led if it is enabled, either normal, hurry, or fast blinking.

#### Syntax
    void blink()
    void blinkHurry()
    void blinkFast()

#### Parameters
None

#### Returns
None

#### See also
[blinkPattern()](#pattern)

[Back to interface](#interface)


<a id="pattern"></a>

## blinkPattern()

#### Description
The method starts patterned blinking mode of an led if it is enabled consisting of periodic bursts of input number of hurry blinks followed by a pause with the led turned off valid for normal blinking.

#### Syntax
    void blinkPattern(byte blinks)

#### Parameters
* **blinks**: Number of blinks in the bursts of hurry blinking.
  * *Valid values*: 2 - 255
  * *Default value*: 3

#### Returns
None

#### See also
[blinkHurry()](#blink)

[Back to interface](#interface)


<a id="restoreMode"></a>

## restoreMode()

#### Description
The method restores the previous led modus or number of blinks in case of patterned blinking.
* It is useful at temporary changing led mode, e.g., to "on" at long lasting blocking operations and then to return to previous (original) mode.

#### Syntax
    void restoreMode()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="access"></a>

## isBlocked(), isFree()

#### Description
The particular getter returns flag determining whether corresponding accessibility of the led is valid.

#### Syntax
    bool isBlocked()
    bool isFree()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding led's accessibility.

#### See also
[block(), free()](#ignore)

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


<a id="state"></a>

## isLit(), isDim()

#### Description
The particular getter returns flag determining whether corresponding led is valid, i.e., the led lights or is dimmed.

#### Syntax
    bool isLit()
    bool isDim()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding led's state.
If the led is blocked, the method always returs false flag.

[Back to interface](#interface)


<a id="visibility"></a>

## isOn(), isOff()

#### Description
The particular getter returns flag determining whether corresponding steady visibility mode of the led is valid.

#### Syntax
    bool isOn()
    bool isOff()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding led's steady visibility.
If the led is blocked, the method always returs false flag.

#### See also
[on(), off(), toggle()](#switch)

[Back to interface](#interface)


<a id="blinking"></a>

## isBlinking(), isPatterned()

#### Description
The particular getter returns flag determining whether corresponding steady or patterned blinking mode of the led is valid.
* In fact, the getter determines whether the internal timer is active and runs.

#### Syntax
    bool isBlinking()
    bool isPatterned()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding led's blinking.
If the led is blocked, the method always returs false flag.

#### See also
[blink(), blinkHurry(), blinkFast()](#blink)

[blinkPattern()](#pattern)

[Back to interface](#interface)


<a id="getStatusFlags"></a>

## getStatusOn(), getStatusOff()

#### Description
The particular getter returns textual flag representing enabled or disabled state of the led.
* The enabled led state is represented with the string "ON".
* The disabled led state is represented with the string "OFF".

#### Syntax
    String getStatusOn()
    String getStatusOff()
    String getStatus()

#### Parameters
None

#### Returns
Textual flag representing enabled or disabled led state.

#### See also
[getStatus()](#getStatus)

[Back to interface](#interface)


<a id="getStatus"></a>

## getStatus()

#### Description
The getter returns textual representation of the current led's state, i.e., flag about its availability.

#### Syntax
    String getStatus()

#### Parameters
None

#### Returns
Textual flag representing current led state.

#### See also
[getStatusOn(), getStatusOff()](#getStatusFlags)

[Back to interface](#interface)


<a id="getModus"></a>

## getMode(), getModeText()

#### Description
The getter returns either numerical or textual representation of the current led's modus, i.e., its current visual function.

#### Syntax
    byte getMode()
    String getModeText()

#### Parameters
None

#### Returns
Numerical or textual representation of current led modus.

#### See also
[getBlinks()](#getBlinks)

[Back to interface](#interface)


<a id="getBlinks"></a>

## getBlinks()

#### Description
The getter returns number of blinks utilized at pattern blinking led modus.
* The getter is usefull only in pattern blinking mode, even if returns the blinks number in either case.

#### Syntax
    byte getBlinks()

#### Parameters
None

#### Returns
Number of blinks used at pattern blinking modus.

#### See also
[getMode(), getModeText()](#getModus)

[Back to interface](#interface)


<a id="forceLit"></a>

## forceLit()

#### Description
The method forcibly turns the led on without changing its current modus, which should not be off mode and led is not blocked or disabled.
* This is useful at the beginning of long blocking operation to ensure the led is physically turned on.

#### Syntax
    void forceLit()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)
