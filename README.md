Button2
======

Arduino/ESP library to simplify working with buttons.

* Author: Lennart Hennigs (https://www.lennarthennigs.de)
* Copyright (C) 2017-2021 Lennart Hennigs.
* Released under the MIT license.

Description
-----------
This library allows you to use callback functions to track single, double, triple and long clicks. It takes care of debouncing. Using this lib will reduce and simplify your source code significantly. 

It has been tested with Arduino, ESP8266 and ESP32 devices.

How To Use
----------

This library allows you to define a button and uses callback functions to detect different types of button interactions.

__Definition__
- Define the button either using the ```constructor``` or the ```begin()``` function.
- Per default the button pins are defined as ```INPUT_PULLUP```. You can override this upon creation.
- On an ESP32 you can use it with the built-in capacitive button pins.


__Callback Handler__
- You can define callback functions to track various types of clicks:
  - ```setTapHandler()``` will be be called when any click occurs.
  - ```setLongClickHandler()``` will be called after the button has released.
  - ```setLongClickDetectedHandler()``` will be called as soon as the long click timeout has passed.
  - ```setDoubleClickHandler()``` and ```setTripleClickHandler()``` detect complex interactions.
  - ```setChangedHandler()```, ```setPressedHandler()``` and ```setReleasedHandler()``` allow to detect basic interactions.

- The callback function needs a ```Button2``` reference parameter. There the reference to the triggered button is stored. This can used to call status fuctions, e.g. ```wasPressedFor()```.

- Please take a look at the included examples to get an overview over the different callback handlers. 

__Timeouts__
- You can define your own timeouts by using these setter functions:

  - ```void setDebounceTime(unsigned int ms);```
  - ```void setLongClickTime(unsigned int ms);```
  - ```void setDoubleClickTime(unsigned int ms);```
    

__The Loop__    
- For the class to work, you need to call the button's `loop()` member function in your sketch's `loop()` function. 
- Please see the examples for more details.

Examples
-----
- [SingleButtonSimple](https://github.com/LennartHennigs/Button2/blob/master/examples/SingleButtonSimple/SingleButtonSimple.ino) – the most basic example, shows how to assign event handlers
- [LongpressHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/LongpressHandler/LongpressHandler.ino) – shows how determine the time of a button press
- [SingleButton](https://github.com/LennartHennigs/Button2/blob/master/examples/SingleButton/SingleButton.ino) – shows the different event handlers
- [MultipleButtons](https://github.com/LennartHennigs/Button2/blob/master/examples/MultipleButtons/MultipleButtons.ino) – how to use two buttons
- [MultiHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandler/MultiHandler.ino) – how to use a single handler for multiple events
- [MultiHandlerTwoButtons](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandlerTwoButtons/MultiHandlerTwoButtons.ino) – a single handler for multiple buttons
- [TrackDualButtonClick](https://github.com/LennartHennigs/Button2/blob/master/examples/TrackDualButtonClick/TrackDualButtonClick.ino) – how to detect when two buttons are clicked at the same time

Notes
-----

- To see the latest changes to the library please take a look at the [Changelog](https://github.com/LennartHennigs/Button2/blob/master/CHANGELOG.md).
 
- And if you find this library helpful, please consider giving it a star at [GitHub](https://github.com/LennartHennigs/Button2). Thanks!



Class Definition
----------------

These are the constructors and the member functions the library provides:

```
Button2();
Button2(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false, boolean activeLow = true);

void begin(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false , boolean activeLow  = true);

void setDebounceTime(unsigned int ms);
void setLongClickTime(unsigned int ms);
void setDoubleClickTime(unsigned int ms);

unsigned int getDebounceTime();
unsigned int getLongClickTime();
unsigned int getDoubleClickTime();
byte getAttachPin();

void setLongClickDetectedRetriggerable(bool retriggerable);

void reset();

void setChangedHandler(CallbackFunction f);
void setPressedHandler(CallbackFunction f);
void setReleasedHandler(CallbackFunction f);

void setTapHandler(CallbackFunction f);
void setClickHandler(CallbackFunction f);
void setDoubleClickHandler(CallbackFunction f);
void setTripleClickHandler(CallbackFunction f);

void setLongClickHandler(CallbackFunction f);
void setLongClickDetectedHandler(CallbackFunction f);

unsigned int wasPressedFor() const;
boolean isPressed() const;
boolean isPressedRaw() const;

byte getNumberOfClicks() const;
byte getClickType() const;

bool operator==(Button2 &rhs);

void loop();
```

Installation
------------
Open the Arduino IDE choose "Sketch > Include Library" and search for "Button2". 
Or download the ZIP archive (https://github.com/lennarthennigs/Button2/zipball/master), and choose "Sketch > Include Library > Add .ZIP Library..." and select the downloaded file.


License
-------

MIT License

Copyright (c) 2017-2021 Lennart Hennigs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
