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
  
  private:
  static const byte PLATOON_MAX        = 5;
  static const byte PLATOON_CONSISTS   = 4;
  static const byte PLATOON_USING_MASK = 0x80;
  static const byte PLATOON_COUNT_MASK = 0x7;

  char spawnYs[PLATOON_MAX];
  byte types[PLATOON_MAX];
  byte timers[PLATOON_MAX];
  byte status[PLATOON_MAX];
};


#define INST_SPAWN_MASK  (B10000000)
#define INST_TYPE_MASK   (B01111111)
#define INST_RAND_MASK   (B11000000)
#define INST_Y_MASK      (B00111111)

#define INST_RAND_WIDE   (B11000000)
#define INST_RAND_NARROW (B10000000)
#define INST_RAND_NONE   (B01000000)
#define INST_RAND_ERROR  (B00000000)

#define INST_END_WAVE    (B00000000)
#define INST_SPAWN(type, rand, y) INST_SPAWN_MASK | (type), (rand) | ((y) & INST_Y_MASK)
#define INST_DELAY(frame)         ((frame) > INST_TYPE_MASK ? INST_TYPE_MASK : (frame))

struct Generator {
  void initialize();
  void spawn(GameLevel& context);
  void draw(GameLevel& context) const;
  inline byte getDifficulty() const { return difficulty; }
  
  private:
  byte difficulty;
  byte zone;
  byte waveCount;
  byte waveIndex;
  byte progCount;
  byte delayTimer;
  byte dispTimer;
};

PROGMEM const byte waveEmpty[] = { INST_DELAY(127), INST_DELAY(127), INST_END_WAVE };
PROGMEM const byte waveBigWall[] = {
  INST_SPAWN(0, INST_RAND_NONE, 56), INST_DELAY(40),
  INST_SPAWN(0, INST_RAND_NONE, 44), INST_DELAY(40),  
  INST_SPAWN(0, INST_RAND_NONE, 32), INST_DELAY(40), 
  INST_SPAWN(0, INST_RAND_NONE, 20), INST_DELAY(40), 
  INST_SPAWN(0, INST_RAND_NONE,  8), INST_DELAY(40), 
  INST_END_WAVE
};

