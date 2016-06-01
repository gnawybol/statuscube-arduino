# Description

Simple controller for one or more status LEDs, controlled via USB CDC.
This implementation is for Adafruit NeoPixels.


# Usage

If needed, change the `PIXELS_PIN` define to set the datapin for the NeoPixels, and `PIXELS_NUM` to change the number of NeoPixels.
Upload the sketch to the Arduino.

Connect to the Arduino at 115200 baud, with eight databits, no parity and one stopbit.

Change the state of a NeoPixel by sending its index and the first letter of the state to set.
The NeoPixels are zero-indexed, so with 5 pixels the first NeoPixel is 0 and the last is 4.
The available states are:
- none: off
- working: pulsing white
- success: green
- fault: yellow
- error: red

Multiple states can be sent in one string, for example `0w1s2f3n`.

Note that Arduinos, by design, get reset every time you connect to its serial port, which will clear the current state of the NeoPixels.
