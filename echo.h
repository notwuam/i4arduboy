#pragma once

#include "gamecore.h"
#include "constants.h"

#define ECHO_GRID_SIZE (3)
#define ECHO_VERT_RESO (SCREEN_HEIGHT / ECHO_GRID_SIZE)
#define ECHO_CYCLE (60)

struct Context;

struct Echo {
  void reset(Context& context, const byte subX);
  void add(const int left, const char top, const char bottom);
  void draw(Context& context);

  private:
  byte intensities[ECHO_VERT_RESO];
  byte submarineX;
  bool acceptFlag;
};

