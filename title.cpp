#include "title.h"
#include "gamecore.h"
#include "util.h"
#include "systembitmaps.h"

int Title::loop(GameCore& core) {
  // controls
  // up
  if(core.pressed(BTN_U) && !prevUpButton) {
    --cursor;
    if(cursor < TITLE_START_GAME) { cursor = TITLE_DISP_RANKING; }
  }
  prevUpButton = core.pressed(BTN_U);
  // down
  if(core.pressed(BTN_D) && !prevDownButton) {
    ++cursor;
    if(cursor > TITLE_DISP_RANKING) { cursor = TITLE_START_GAME; }
  }
  prevDownButton = core.pressed(BTN_D);
  // enter
  bool enterButton = core.pressed(BTN_A) || core.pressed(BTN_B);
  if(enterButton && !prevEnterButton) {
    if(cursor == TITLE_TOGGLE_SOUND) {
      core.soundOn = !core.soundOn;
    }
    else {
      prevEnterButton = true;
      return TITLE_START_GAME;
    }
  }
  prevEnterButton = enterButton;

  // drawing
  draw(core);
  
  return TITLE_NOINPUT;
}

void Title::draw(GameCore& core) {
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
}

