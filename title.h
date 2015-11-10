#pragma once

#include "gamecore.h"
#include "util.h"
#include "systembitmaps.h"

enum {
  TITLE_NOINPUT = 0,
  TITLE_START_GAME,
  TITLE_TOGGLE_SOUND,
  TITLE_DISP_RANKING,
};

typedef unsigned char byte;
struct GameCore;

struct Title {
  byte loop(GameCore& core) {
    // controls
    // up
    if(core.pushed(BTN_U)) {
      --cursor;
      if(cursor < TITLE_START_GAME) { cursor = TITLE_DISP_RANKING; }
    }
    // down
    if(core.pushed(BTN_D)) {
      ++cursor;
      if(cursor > TITLE_DISP_RANKING) { cursor = TITLE_START_GAME; }
    }
    // enter
    if(core.pushed(BTN_A) || core.pushed(BTN_B)) {
      if(cursor == TITLE_TOGGLE_SOUND) {
        core.soundOn = !core.soundOn;
      }
      else {
        return TITLE_START_GAME;
      }
    }
  
    // === drawing ===
    // logo and background
    core.drawBitmap(10, SCREEN_HEIGHT/2 - bitmapLogo[1]/2 - 1, bitmapLogo, 1);
    DrawWave(core, 0, core.frameCount());
    DrawBottom(core, 0);
    
    // START
    core.setCursor(70, 15);
    core.print("START");
    // SOUND ON/OFF
    core.setCursor(70, 28);
    core.print("SOUND");
    core.setCursor(105, 28);
    if(core.soundOn) { core.print("ON"); }
    else             { core.print("OFF"); }
    // RANKING
    core.setCursor(70, 41);
    core.print("RANKING");
  
    // cursor
    core.setCursor(63, (cursor-1) * 13 + 15);
    core.print(">");
    
    return TITLE_NOINPUT;
  }

  private:
  byte cursor = TITLE_START_GAME;
};

