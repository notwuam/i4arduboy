#pragma once

#include "Arduboy.h"

#define DEBUG

#define SCREEN_WIDTH (WIDTH)
#define SCREEN_HEIGHT (HEIGHT)

#define BTN_A (A_BUTTON)
#define BTN_B (B_BUTTON)
#define BTN_U (UP_BUTTON)
#define BTN_D (DOWN_BUTTON)
#define BTN_L (LEFT_BUTTON)
#define BTN_R (RIGHT_BUTTON)

struct GameCore {
  bool soundOn = true;
  
  void setup() {
    arduboy.start();
    arduboy.setFrameRate(60);
  }
  
  inline bool nextFrame() { return arduboy.nextFrame(); }
  inline void clearDisplay() { arduboy.clearDisplay(); }
  inline void display() { arduboy.display(); }
  inline int frameCount() { return arduboy.frameCount; }
  inline bool pressed(byte button) { return arduboy.pressed(button); }
  inline void drawPixel(int x, int y, byte c) { arduboy.drawPixel(x, y, c); }
  inline void drawBitmap(int x, int y, const byte* bitmap, byte c) {
    arduboy.drawBitmap(x, y, bitmap+2, pgm_read_byte(bitmap), pgm_read_byte(bitmap+1), c);
  }
  inline void fillRect(int x, int y, int w, int h, byte c) {
    arduboy.fillRect(x, y, w, h, c);
  }
  inline void drawCircle(int x, int y, int r, byte c) {
    arduboy.drawCircle(x, y, r, c);
  }
  inline void setCursor(int x, int y) { arduboy.setCursor(x, y); }
  inline void print(char* text) { arduboy.print(text); }
  void playScore(const byte* score) {
    if(soundOn) {
      arduboy.tunes.playScore(score);
    }
  }
  void tone(const unsigned int f, const unsigned long d) {
    if(soundOn) {
      arduboy.tunes.tone(f, d);
    }
  }

#ifdef DEBUG
  inline Arduboy& getArduboy() { return arduboy; }
#endif

  private:
  Arduboy arduboy;
};

