#pragma once

enum {
  TITLE_NOINPUT = 0,
  TITLE_START_GAME,
  TITLE_TOGGLE_SOUND,
  TITLE_DISP_RANKING,
};

struct GameCore;

struct Title {
  int loop(GameCore& core);

  private:
  void draw(GameCore& core);
  
  private:
  unsigned char cursor = TITLE_START_GAME;
  bool prevEnterButton = true;
  bool prevUpButton    = true;
  bool prevDownButton  = true;
};

