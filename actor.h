#pragma once

#include "constants.h"

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
  static const char W = 10;
  static const char H = 4;
  
  bool prevFire;
  char extraLives;
  unsigned char armer;

  void initialize();
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Torpedo : public Actor {
  static const char W = 10;
  static const char H = 1;

  float vx;

  void initialize();
  bool tryLaunch(const float sx, const float sy);
  void move(Context& context);
  void draw(Context& context);
  void onHit() { inactivate(); }
};

struct AutoShot : public Actor {
  static const char W = 12;
  static const char H = 10;

  void initialize(const float x, const float y) { activate(x, y); }
  void move(Context& context);
  void draw(Context& context);
  void onHit() { inactivate(); }
};

struct BigEnemy : public Actor {
  static const char W = 18;
  static const char H = 6;

  bool grazed;
  unsigned char timer;

  void initialize(const float y);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
  void onGraze() { grazed = true; }
};

struct SmallEnemy : public Actor {
  static const char W = 2;
  static const char H = 2;

  unsigned char type;
  unsigned char timer;

  void initialize(const float y, const unsigned char type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);

  private:
  static const unsigned char ZIG_PERIOD = 96;
  static const unsigned char TRI_PERIOD = 128;
};

struct Bullet : public Actor {
  static const char W = 1;
  static const char H = 1;
  
  float angle;
  unsigned char type;
  
  void initialize(const float sx, const float sy, const float radian, const unsigned char type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Particle : public Actor {
  unsigned char type;
  unsigned char limit;

  void initialize(const float x, const float y, const unsigned char type);
  void move(Context& context);
  void draw(Context& context);
};

