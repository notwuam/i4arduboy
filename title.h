#pragma once

#define TITLE_NOINPUT (0)
#define TITLE_START_GAME (1)
#define TITLE_TOGGLE_SOUND (2)
#define TITLE_DISP_RANKING (3)

struct GameCore;

struct Title {
  int loop(GameCore& core);

  private:
  void draw(GameCore& core);
  
  private:
  unsigned char cursor = TITLE_START_GAME;
  bool prevEnterButton = true;
  bool prevUpButton = true;
  bool prevDownButton = true;
};

