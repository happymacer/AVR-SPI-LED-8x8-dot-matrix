# AVR-SPI-LED-8x8-dot-matrix on baremetal ATMEGA328P
I need to write to a SD card on an AVR ATMEGA328P, so I got the SPI interface working and proved it by writing to a 8x8 matrix. 

This is not a library for 8x8 matrix use - please look elsewhere for that.  This code is not pretty, and is definitely not optimal.  It is simply written and heavily annotated for ease of understanding.  I want the annotation as a memory prompt for me in future when I reuse the SPI code for something else.  Im sharing it in case its helpful for someone else but also as a free place to save my code. :-) 

The code includes examples just for the fun of it.

The code is garden variety C, developed in the Microchip Studio 7 IDE.  To run the code, make sure that the IDE is installed and working correctly, then download and double click the __8x8matrix_spi.atsln__ file.  Everything should load in the IDE and you should be able to compile and upload the hex file to your ATMEGA328P baremetal.  I use a USBASP programmer and a breakout board to make it simple.  You may need to add F_CPU=8000000 if your chip crystal is 8MHz to the project properties. Its defaulted to 8MHz. 

Connect the 8x8 matrix module (it must have a MAX7219 or a MAX7221 chip) as follows:

Module --> Breakout board
*   VCC --> +5V  
*   GND --> GND 
*   DIN --> MOSI (pin PB3)
*   CS  --> SS (pin PB2)
*   CLK --> SCLK (pin PB5)

The MAX7219 is not designed for SPI but it works OK.

* Typical 8x8 matrix: https://www.jaycar.com.au/arduino-compatible-8-x-8-led-dot-matrix-module/p/XC4499
* Typical breakout board: https://www.aliexpress.com/item/32247504235.html?src=bing&albslr=201492522&isdl=y

