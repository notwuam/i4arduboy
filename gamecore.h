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
    arduboy.print("Hello World");
    arduboy.display();
  }
  
  inline bool nextFrame() { return arduboy.nextFrame(); }
  inline void clearDisplay() { arduboy.clearDisplay(); }
  inline void display() { arduboy.display(); }
  inline int frameCount() { return arduboy.frameCount; }
  inline bool pressed(unsigned char button) { return arduboy.pressed(button); }
  inline void drawPixel(int x, int y, unsigned char c) { arduboy.drawPixel(x, y, c); }
  inline void drawBitmap(float x, float y, const unsigned char* bitmap, unsigned char c) {
    arduboy.drawBitmap(round(x), round(y), bitmap+2, pgm_read_byte(bitmap), pgm_read_byte(bitmap+1), c);
  }
  inline void fillRect(int x, int y, int w, int h, unsigned char c) {
    arduboy.fillRect(x, y, w, h, c);
  }
  inline void setCursor(int x, int y) { arduboy.setCursor(x, y); }
  inline void print(char* text) { arduboy.print(text); }
  void playScore(const unsigned char* score) {
    if(soundOn) {
      arduboy.tunes.playScore(score);
    }
  }

#ifdef DEBUG
  inline Arduboy& getArduboy() { return arduboy; }
#endif

  private:
  Arduboy arduboy;
};

