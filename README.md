# DiverInterface

Introduction
------------------------

This is the wearable diver interface program designed for TinyScreen. This code allows the user to acoustically range other divers. This device uses several compnenets as detailed below.

------------------------

Components
----------------------

This code is built for the tiny circuits, tiny screen package and uses their corresponding libraries. It uses the Bosch BMA250 3-axis accelerometer shield, the Honeywell HMC5883L 3-axis compass shields, and acoustic Nanomodems. A RS232 to TTL converter is needed for acoustic modem interaction. Both the  acoustic modems and watch are powered by a 5 volt power source.

----------------------

Libraries
---------------------

#include <Wire.h> //https://www.arduino.cc/en/Reference/Wire

#include <SPI.h> //https://www.arduino.cc/en/Reference/SPI

#include <TinyScreen.h> //https://github.com/TinyCircuits/TinyCircuits-TinyScreen_Lib

#include <TimeLib.h> //https://playground.arduino.cc/Code/Time

#include <SoftwareSerial.h> //https://github.com/PaulStoffregen/SoftwareSerial

#include "BMA250.h" //https://tinycircuits.com/products/accelerometer-tinyshield#downloads-anchor

---------------------

Nanomodems
----------

These modems are provided by Jeff Neasham of Newcastle University, School of Electrical and Electronic Egineering. the following is his introduction to the Nanomodems.

----------------------

Nanomodems are a low cost, miniature acoustic communication and ranging device for
underwater vehicles, divers and subsea instruments. Short data messages may be exchanged
between units and an efficient “ping” protocol is implemented for range measurement by
transponder operation. If multiple units are deployed in known locations, then long baseline
positioning (LBL) operation is possible.

----------------------


Boot Up
----------

Upon boot the dive watch will allow for the storage of several modem addresses. These can be changed to the approiate addresses using the top two (left and right) buttons on the watch and saved accordingly. There is no need to save the adress of the attached modem in the watch. The clock will then be set using the same method as the modem addresses. Upon boot it is necessary to calibrate the compass. This is done by roatating the compass along the three axes (X, Y, Z). The watch is ready for action.

-------------------

Operation
-----------

The watch will allow for acoustic ranging via the minimodems. The upper left button will call this function. It will ask which partner modem to contact. A screen will display succesful function activation. Then the return distance will be displayed. Pressing the indicated button returns the user to the original screen.

---------------------

Liscense
----------

MIT License

Copyright (c) [2018] [Yelena Randall && Justin Lewis] 
[University Of Rhode Island]
Sponsored by [ Prof. Bridget Buxton]

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

-------------------

Special Thanks
--------------

Prof. Stephen Licht, University of Rhode Island

Prof. Bridget Buxton, University of Rhode Island

Jeff Neasham, Newcastle University

--------------------------
