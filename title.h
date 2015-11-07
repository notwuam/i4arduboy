#pragma once

enum {
  TITLE_NOINPUT = 0,
  TITLE_START_GAME,
  TITLE_TOGGLE_SOUND,
  TITLE_DISP_RANKING,
};

typedef unsigned char byte;
struct GameCore;

struct Title {
  byte loop(GameCore& core);

  private:
  void draw(GameCore& core);
  
  private:
  byte cursor = TITLE_START_GAME;
  bool prevEnterButton = true;
  bool prevUpButton    = true;
  bool prevDownButton  = true;
};

