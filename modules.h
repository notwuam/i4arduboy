#pragma once

#include "gamecore.h"
#include "constants.h"

struct GameLevel;

#define ECHO_CYCLE (60)

struct Echo {
  void reset(GameLevel& context, const byte subX);
  void add(const int left, const char top, const char bottom);
  void draw(GameLevel& context) const;

  private:
  static const byte ECHO_GRID_SIZE = 3;
  static const byte ECHO_VERT_RESO = SCREEN_HEIGHT / ECHO_GRID_SIZE;
  static const byte ECHO_HORI_RESO = 16;

  byte intensities[ECHO_VERT_RESO];
  byte submarineX;
  bool acceptFlag;
};


struct Platoons {
  void initialize();
  void set(const char y, const byte type);
  void spawn(GameLevel& context);
  bool checkBonus(const byte idx, bool killed);
  
  private:
  inline bool inUse(const byte idx) const {
    return (status[idx] & PLATOON_USING_MASK) != 0;
  }
  
  static const byte PLATOON_MAX        = 5;
  static const byte PLATOON_CONSISTS   = 4;
  static const byte PLATOON_USING_MASK = 0x80;
  static const byte PLATOON_COUNT_MASK = 0x7;

  char spawnYs[PLATOON_MAX];
  byte types[PLATOON_MAX];
  byte timers[PLATOON_MAX];
  byte status[PLATOON_MAX];
};


struct Generator {
  void initialize();
  void spawn(GameLevel& context);
  void draw(GameLevel& context) const;
  inline byte getDifficulty() const { return difficulty; }
  
  private:
  static const byte ZONE_DISP_FRAMES = 90;
  static const byte WAVES_IN_ZONE    = 10;
  
  byte difficulty;
  byte zone;
  byte wave;
  byte progCounter;
  byte delayTimer;
  byte dispTimer;
};

