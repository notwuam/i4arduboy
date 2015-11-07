#pragma once

#include "constants.h"

typedef unsigned char byte;
struct Context;

struct Actor {
  float x = EXIST_THRESHOLD;
  float y;

  void activate(const float x, const float y) {
    this->x = x;
    this->y = y;
  }
  inline void inactivate() {
    x = EXIST_THRESHOLD;
  }
  inline bool exist() const {
    return x > EXIST_THRESHOLD;
  }
};

struct Submarine : public Actor {
  static const byte W = 10;
  static const byte H = 4;
  
  bool prevFire;
  char extraLives;
  byte armer;

  void initialize();
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Torpedo : public Actor {
  static const byte W = 10;
  static const byte H = 1;

  float vx;

  void initialize();
  bool tryLaunch(const float sx, const float sy);
  void move(Context& context);
  void draw(Context& context);
  void onHit() { inactivate(); }
};

struct AutoShot : public Actor {
  static const byte W = 12;
  static const byte H = 10;

  void initialize(const float x, const float y) { activate(x, y); }
  void move(Context& context);
  void draw(Context& context);
  void onHit() { inactivate(); }
};

struct BigEnemy : public Actor {
  static const byte W = 18;
  static const byte H = 6;

  bool grazed;
  byte timer;

  void initialize(const float y);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
  void onGraze() { grazed = true; }
};

struct SmallEnemy : public Actor {
  static const byte W = 2;
  static const byte H = 2;

  byte type;
  byte timer;

  void initialize(const float y, const byte type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);

  private:
  static const byte ZIG_PERIOD = 96;
  static const byte TRI_PERIOD = 128;
};

struct Bullet : public Actor {
  static const byte W = 1;
  static const byte H = 1;
  
  float angle;
  byte type;
  
  void initialize(const float sx, const float sy, const float radian, const byte type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Particle : public Actor {
  byte type;
  byte limit;

  void initialize(const float x, const float y, const byte type);
  void move(Context& context);
  void draw(Context& context);
};

