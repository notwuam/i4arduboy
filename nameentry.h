#pragma once

#include "gamecore.h"
#include "scores.h"

struct NameEntry {
  void initialize(byte rank, unsigned int score) {
    name[0] = name[1] = name[2] = ' ';
    nameCursor  = 0;
    kbCursor    = 10;
    this->rank  = rank;
    this->score = score;
    exitCount   = -1;
  }

  bool loop(GameCore& core) {
    static const byte COLUMN = 10;
    static const byte ROW    = 4;
    if(exitCount < 0) {
      // move cursor
      if(core.pushed(BTN_U)) {
        if(kbCursor / COLUMN == 0) { kbCursor += COLUMN * (ROW - 1); }
        else { kbCursor -= COLUMN; }
      }
      if(core.pushed(BTN_D)) {
        if(kbCursor / COLUMN == ROW - 1) { kbCursor -= COLUMN * (ROW - 1); }
        else { kbCursor += COLUMN; }
      }
      if(core.pushed(BTN_L)) {
        if(kbCursor % COLUMN == 0) { kbCursor += COLUMN - 1; }
        else { kbCursor -= 1; }
      }
      if(core.pushed(BTN_R)) {
        if(kbCursor % COLUMN == COLUMN - 1) { kbCursor -= COLUMN - 1; }
        else { kbCursor += 1; }
      }
      // entry character
      if(core.pushed(BTN_A)) {
        if(kbCursor < 36 && nameCursor < 3) {
          entryChar(core, kbCursor < 10 ? kbCursor + '0' : kbCursor + 'A' - 10);
        }
        else if(kbCursor == BACKSPACE) {
          backspace(core);
        }
        else if(kbCursor == ENTER_KEY) {
          exitCount = 0;
          core.playScore(bing); // ToDo: another sfx
        }
        else if(nameCursor < 3) {
          entryChar(core, ' ');
        }
      }
      // b button BS
      if(core.pushed(BTN_B)) {
        backspace(core);
      }
    }
    
    // === draw ===
    char text[6];
    text[1] = '\0';
    // keyboard
    {
      static const byte W  = 10;
      static const byte H  = 12;
      static const byte BX = (SCREEN_WIDTH - (W * (COLUMN - 1) + 6)) / 2;
      static const byte BY = 20;
      // number
      for(char i = 0; i < 10; ++i) {
        text[0] = '0' + i;
        core.setCursor(i * W + BX, BY);
        core.print(text);
      }
      // alphabet
      for(char i = 0; i < 26; ++i) {
        text[0] = 'A' + i;
        core.setCursor(i % COLUMN * W + BX, (i / COLUMN + 1) * H + BY);
        core.print(text);
      }
      // backspace
      text[0] = 27;
      core.setCursor(7 * W + BX, 3 * H + BY);
      core.print(text);
      // enter (ToDo)
      // cursor
      if(exitCount < 0) {
        core.drawCircle(kbCursor % COLUMN * W + BX + 2, kbCursor / COLUMN * H + BY + 3, 5, 1);
      }
    }
    // entry
    {
      // rank
      static const byte BY = 5;
      text[0] = rank + '1';
      core.setCursor(24, BY);
      core.print(text);
      // name
      static const byte NX = 42;
      static const byte NW = 8;
      for(byte i = 0; i < 3; ++i) {
        if(exitCount < 0 || core.frameCount() / 30 % 2 == 0) {
          text[0] = name[i];
          core.setCursor(i * NW + NX, BY);
          core.print(text);
        }
        // underline
        if(i != nameCursor || core.frameCount() / 30 % 2 == 0) {
          core.fillRect(i * NW + NX, BY + 8, 5, 1, 1);
        }
      }
      // score
      sprintf(text, "%05d", score);
      core.setCursor(76, BY);
      core.print(text);
    }

    // exit
    if(exitCount >= 0) { ++exitCount; }
    if(exitCount >= 150) { return true; }
    return false;
  }

  inline unsigned int getScore() const { return score; }
  inline const char* getName() const { return name; }
  
  private:
  void entryChar(GameCore& core, const char c) {
    name[nameCursor] = c;
    ++nameCursor;
    if(nameCursor >= 3) { kbCursor = ENTER_KEY; }
    core.tone(880, 100);
  }
  void backspace(GameCore& core) {
    if(nameCursor > 0) { --nameCursor; }
    name[nameCursor] = ' ';
    core.tone(220, 100);
  }
  
  static const byte BACKSPACE = 37;
  static const byte ENTER_KEY = 39;
  
  char name[3];
  byte nameCursor;
  byte kbCursor;
  byte rank;
  unsigned int score;
  int  exitCount;
};

