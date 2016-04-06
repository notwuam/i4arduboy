# i4arduboy

Porting a shmup **"i-20000"** to [Arduboy](https://www.arduboy.com/).
Original version (for Windows) we made is [here](http://playism-games.com/game/216/i-20000).

You need an [Arduboy](https://www.arduboy.com/) to play.
Or you have an [Arduino](https://www.arduino.cc/) board and modify a source code, so you may be able to play ([more details](#other_hardwares)).

[日本語はこちら](./README_JP.md).

## Media

[Demo playing on Youtube.](https://www.youtube.com/watch?v=KDbjBe9sMog)

## How to install

1. Install [ArduinoIDE](https://www.arduino.cc/en/Main/Software).
2. Download [ArduboyLib](https://github.com/Arduboy/Arduboy) and install.
3. Clone or Download this repository. If you download the ZIP, extract it.
4. Connect your Arduboy to a PC with a USB cable.
5. Open "i4arduboy.ino" with ArduinoIDE.
6. Click "Upload" button at the upper left.
7. Enjoy!

## Instructions

* **Weapons**: you can use guns and topedoes to defeat enemies.
  + **Guns** fire automatically. You can destroy small enemies with them.
  + **Torpedoes** are launched when you push A or B button. This is the only way to defeat big enemies.
  + Only one torpedo can be launch at a time.
* **Game Field**: there is **the invisible area** in game field.
  + Enemies may exist as far out as two screens away from the screen you can see.
  + Torpedoes can attack them in this area.
  + But a miss of torpedo's attack will alert the target, so it will close in...
* **Sonar**: in order to know where enemies locate at.
  + Dots flash on the right side of the screen periodically; these indicate locations of enemy.

## Controls

* **Directional buttons**: Move submarine / Move cursor
* **Button A**: Launch torpedo / Confirm
* **Button B**: Launch torpedo / Cancel
* A+B+Left (only in level): Return to title

## Build options

There are some build options in ```constants.h``` you can configure with macros.

* ```LOW_FLASH_MEMORY``` - If in short flash memory (program space), you should define this macro in order to get memory space (several hundred bytes) instead of omitting some features.
* ```USE_RANKING_EEPROM``` - If you want to keep the score ranking while power off, define this. This feature uses EEPROM and several hundred bytes in flash memory.
* ```DISABLE_MUSIC``` - If you define this, flash memory get a margin of about 500 bytes instead of music score data.

In addition, you can configure constant numbers involved with the game level, e.g. maximums of each characters, your submarine's extra lives, etc.

## <a name="other_hardwares">How about other Arduino hardwares?

You have another ways in order to output an image from Arduino, e.g. graphics LCDs, composite video output.

If you display i4arduboy with these devices, please re-implement the following functions and macros in ```gamecore.h```.

* ```SCREEN_WIDTH``` - Output screen width [pixels]
* ```SCREEN_HEIGHT``` - Output screen height [pixels]
* ```void setup()``` - This function is called once immediately after boot. You can initialize output modules here.
* ```void updateInput()``` - This function is called at the beginning of the main loop.Check the current input and store previous one.
* ```bool pressed(byte button)``` - Returns ```true``` for pressed state. ```button```: a button code. It is defined like ```BTN_X``` as macro.
* ```bool pushed(byte button)``` - Returns ```true``` for "just" pushed state. ```button```: a button code. It is defined like ```BTN_X``` as macro.
* ```bool nextFrame()``` - Returns ```false``` while waiting for next frame. This function is called by the main loop in order to control the frame rate.
* ```void clearDisplay()``` - Resets display to black.
* ```void display()``` - Shows what has draw in this frame.
* ```int frameCount()``` - Get the number of ```nextFrame``` returns ```true```.
* ```void drawPixel(int x, int y, byte c)``` - Draw a pixel at (x,y). ```c```: color, 0 is black, 1 is white.
* ```void drawBitmap(int x, int y, const byte* bitmap, byte c)``` - Draw a bitmap image data (```byte``` array) in PROGMEM. ```(x,y)```: the destination (top-left). ```bitmap```: the source data (the format is as below). ```c```: color, 0 is black, 1 is white and 2 is invert.
  + offset 0, size 1: width [0-255]
  + offset 1, size 1: height [0-255]
  + offset 2, size (width * height): 
    1 byte expresses 8 pixels up-to-down (corresponding to LSB-to-MSB). 0 is black, 1 is white. Data goes to next low every [width] bytes.
* ```void fillRect(int x, int y, int w, int h, byte c)``` - Fills a rectangle area of (x,y)-(x+w-1,y+h-1). ```c```: color, 0 is black, 1 is white.
* ```void drawCircle(int x, int y, int r, byte c)``` - Draws a circle. ```(x,y)```: center. ```r```: radius.  ```c```: color, 0 is black, 1 is white.
* ```void setCursor(int x, int y)``` - Set the position of cursor for ```print(char *)```.
* ```void print(char* text)``` - Print a text at the position set with ```setCursor(int, int)```.
* ```bool playing()``` - Returns ```true``` for playing a music score.
* ```void playScore(const byte* score)``` - Play a music score data (```byte``` array) in PROGMEM. About the format details, refer to  [miditones](https://github.com/LenShustek/miditones).
* ```void stopScore()``` - Stop playing the music score (if it is playing).
* ```void tone(unsigned int f, unsigned long d)``` - Beep. ```f```: frequency [Hz]. ```d```: during [ms].

You can find output libraries in [ArduinoPlayground/Output](http://playground.arduino.cc/Main/InterfacingWithHardware#Output).

## Technical informations

Coming soon.
