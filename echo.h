#pragma once

#include "gamecore.h"
#include "constants.h"

#define ECHO_GRID_SIZE (3)
#define ECHO_VERT_RESO (SCREEN_HEIGHT / ECHO_GRID_SIZE)
#define ECHO_CYCLE (60)

struct Context;

struct Echo {
  void reset(Context& context, const float subX);
  void add(const float left, const float top, const float bottom);
  void draw(Context& context);

  private:
  unsigned char distToIntensity(const float dist) const {
    return (unsigned char)((FIELD_WIDTH - dist) / 20);
  }

  private:
  unsigned char intensities[ECHO_VERT_RESO];
  unsigned char submarineX;
  bool acceptFlag;
};

