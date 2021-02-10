Button2
======

* Author: Lennart Hennigs (https://www.lennarthennigs.de)
* Copyright (C) 2017-2021 Lennart Hennigs.
* Released under the MIT license.

Arduino Library to simplify working with buttons.


Description
-----------
It allows you to use callback functions to track single, double, triple and long clicks. It also takes care of debouncing. It will reduce and simplify your source code significantly. 

Tested with Arduino, ESP8266 and ESP32.

How To Use
----------

Please take a look at the included examples to get an overview over the different callback handlers. 

- The callback function from ```setTapHandler()```  will be called when any click (single, double, triple) occurs.
- The callback function from  ```setLongClickDetectedHandler()``` will be called as soon as the long click timeout has passed.

- The callback for ```setLongClickHandler()``` will be called after the button has released.

- You can define your own timeouts by using these setter functions before you include the class:

  ```void setDebounceTime(unsigned int ms);```
  
  ```void setLongClickTime(unsigned int ms);```
  
  ```void setDoubleClickTime(unsigned int ms);```
    
**Note:** For the class to work, you need to call the `loop()` member function in your sketch's `loop()` function. See the example for more details.


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

And please take a look at the [Changelog](https://github.com/LennartHennigs/Button2/blob/master/CHANGELOG.md) for the latest changes to the library.

Installation
------------
Open the Arduino IDE choose "Sketch > Include Library" and search for "Button2". 
Or download the ZIP archive (https://github.com/lennarthennigs/Button2/zipball/master), and choose "Sketch > Include Library > Add .ZIP Library..." and select the downloaded file.


License
-------

MIT License

Copyright (c) 2017-2020 Lennart Hennigs

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
