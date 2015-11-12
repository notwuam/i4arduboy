# i4arduboy

Porting a shmup **"i-20000"** to [Arduboy](https://www.arduboy.com/).
Original version (for Windows) we made is [here](http://playism-games.com/game/216/i-20000).

You need an [Arduboy](https://www.arduboy.com/) to play.
Or you have an [Arduino](https://www.arduino.cc/) board and modify a couple of source codes, so you may be able to play.

[日本語はこちら](./README_JP.md).

## Media

[Demo playing on Youtube.](https://www.youtube.com/watch?v=KDbjBe9sMog)

## How to install

1. Install [ArduinoIDE](https://www.arduino.cc/en/Main/Software).
2. Download [ArduboyLib](https://github.com/Arduboy/Arduboy) and install ([more details](https://github.com/Arduboy/Arduboy#how-to-use-the-library)).
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

## How about other Arduino hardwares?

Coming soon.

## Technical informations

Coming soon.
