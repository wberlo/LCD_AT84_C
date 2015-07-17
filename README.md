# LCD_AT84_C
c version of the liquid crystal library for arduino, adapted for ATtiny 84

ATtiny84 port B (PB0..2) is used for control
Port A (PA0..3) is used for data; only 4 bit data communication is used.
Compiled with Atmel Studio 6.2

Attiny clock frequency 8 MHz, no prescaler.
This is not neccessary, but I used the library in an application that also used serial communication. This application was for 8 MHz clock.
