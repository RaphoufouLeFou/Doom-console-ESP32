# The console
The console is an esp32 based console, made to run doom II and other games.
The main board is a custom PCB, as well as the case of the console.
all is 3D printed

The console has an 1800 mAh battery, lasting in theroy for more than 15h of gaming

Main processor : esp32-S3 with 8Mb of flash, and 8Mb of RAM, dual core, running at 240Mhz.

The screen is an LCD 2.4' SPI display, with a resolution of 320*240, and 16 bits colors.

# Doom

The doom port for the esp32 was originaly created by ESPRESSIF, but i have modified it.

Due to the limited storage avalable on the console, we are limited to one world at a time saved on the console.

Doom run on one core of the ESP32, the other core is reserved to the display.

Doom run at a nearly constant 35 fps, whith is pretty good

# Pictures

![Pic1](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Pictures/IMG20230617225416.jpg)

![Pic2](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Pictures/IMG20230617225423.jpg)

![Pic3](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Pictures/IMG20230617225437.jpg)

![Pic4](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Pictures/IMG20230617225511.jpg)

![Pic5](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Pictures/IMG20230617225515.jpg)

![Pic6](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Pictures/IMG20230617225528.jpg)

# Shematics

![shematic1](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Shematics/Sheet_1.png)

![shematic2](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Shematics/Sheet_2.png)

# Mainboard

![Board1](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Board/PCB_PCB_doom%20pc%202_3_2023-06-17.png)

![Board2](https://github.com/RaphoufouLeFou/Doom-console-ESP32/blob/main/Console/Board/PCB_PCB_doom%20pc%202_3_2023-06-172.png)

# Render-Test

This app is and early prototype of an minecraft like blocky game whith my own render engine. It's preaty bad for the moment and it require a lot of new functionalities and optimisations.

For the moment, the engine can only render cubes with any x y z positions and x y z sizes, with any color in the 16 bits palette of the LCD screen.

I've tested it with 55 cubes to make a little house, but it only go up to 20-25 fps, but it's still kinda cool.
