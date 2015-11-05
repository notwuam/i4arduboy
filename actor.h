#pragma once

#include "constants.h"

struct Context;

struct Actor {
  float x = EXIST_THRESHOLD;
  float y;

  void activate(float x, float y) {
    this->x = x;
    this->y = y;
  }
  inline void inactivate() {
    x = EXIST_THRESHOLD;
  }
  inline bool exist() {
    return x > EXIST_THRESHOLD;
  }
};

struct Submarine : public Actor {
  static const int W = 10;
  static const int H = 4;
  
  bool prevFire = false;
  char extraLives = START_LIVES;
  unsigned char armer = ARMER_FRAMES;

  void initialize();
  void move(Context& context);
  void draw(Context& context);
  void onHit();
};

struct Torpedo : public Actor {
  static const int W = 10;
  static const int H = 1;

  float vx = 0.f;

  void initialize();
  bool tryLaunch(float sx, float sy);
  void move(Context& context);
  void draw(Context& context);
  void onHit() {
    inactivate();
  }
};

struct BigEnemy : public Actor {
  static const int W = 18;
  static const int H = 6;

  bool grazed = false;

  void initialize(float y);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
  void onGraze() { grazed = true; }
};

struct Bullet : public Actor {
  static const int W = 1;
  static const int H = 1;
  
  float angle;
  unsigned char type = 0;
  
  void initialize(float sx, float sy, float radian, unsigned char type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Particle : public Actor {
  unsigned char type;
  unsigned char limit;

  void initialize(float x, float y, unsigned char type);
  void move(Context& context);
  void draw(Context& context);
};

