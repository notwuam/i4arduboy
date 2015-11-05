#pragma once

#include "gamecore.h"
#include "constants.h"

#define ECHO_GRID_SIZE (3)
#define ECHO_VERT_RESO (SCREEN_HEIGHT / ECHO_GRID_SIZE)
#define ECHO_CYCLE (60)

struct Context;

struct Echo {
  void reset(Context& context, float subX);
  void add(float left, float top, float bottom);
  void draw(Context& context);

  private:
  unsigned char distToIntensity(float dist) const {
    return (unsigned char)((FIELD_WIDTH - dist) / 20);
  }

  private:
  unsigned char intensities[ECHO_VERT_RESO];
  bool acceptFlag;
  float submarineX;
};

