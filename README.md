# The console
The console is an esp32 based console, made to run doom II and other games.
The main board is a custom PCB, as well as the case of the console.
all is 3D printed

The console has an 1800 MAh battery, lasting in theroy for more than 15h of gaming

Main processor : esp32-S3 with 8Mb of flash, and 8Mb of RAM, dual core, running at 240Mhz.

The screen is an LCD 2.7' SPI display, with a resolution of 320*240, and 16 bits colors.

# Doom

The doom port for the esp32 was originaly created by ESPRESSIF, but i have modified it.

Due to the limited storage avalable on the console, we are limited to one world at a time saved on the console.

Doom run on one core of the ESP32, the other core is reserved to the display.

Doom run at a nearly constant 35 fps, whith is pretty good

# Pictures

![shematic](https://github.com/RaphoufouLeFou/Casio-message-and-connect/blob/main/Images/Connect-3.jpg)
