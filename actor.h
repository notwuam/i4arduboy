#pragma once

#include "constants.h"

typedef unsigned char byte;
typedef int fixed;
struct Context;

struct Actor {
  float x;
  float y;

  inline void activate(const float x, const float y) {
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

struct Submarine {
  static const byte W = 10;
  static const byte H = 4;
  
  fixed x, y;
  bool  prevFire;
  char  extraLives;
  byte  armer;

  inline void inactivate() { x = -(64 << 8); }
  inline bool exist() const { return x > -(64 << 8); }
  inline char fieldX() const { return x >> 8; }
  inline char fieldY() const { return y >> 8; }
  
  void initialize();
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Torpedo {
  static const byte W = 10;
  static const byte H = 1;

  int   x;
  char  y;
  fixed vx;

  inline void inactivate() { x = EXIST_THRESHOLD; }
  inline bool exist() const { return x > EXIST_THRESHOLD; }
  
  void launch(const char x, const char y);
  void move(Context& context);
  void draw(Context& context);
  void onHit() { inactivate(); }
};

struct AutoShot {
  static const byte W = 12;
  static const byte H = 10;

  char x, y;

  inline void inactivate() { x = EXIST_THRESHOLD; }
  inline bool exist() const { return x > EXIST_THRESHOLD; }
  void initialize(const char x, const char y) {
    this->x = x;
    this->y = y;
  }
  void move(Context& context);
  void draw(Context& context);
  void onHit() { inactivate(); }
};

struct BigEnemy {
  static const byte W = 18;
  static const byte H = 6;

  int  x;
  char y;
  bool grazed;
  byte timer;

  inline void inactivate() { x = EXIST_THRESHOLD; }
  inline bool exist() const { return x > EXIST_THRESHOLD; }
  void initialize(const char y);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
  void onGraze() { grazed = true; }
};

struct SmallEnemy {
  static const byte W = 2;
  static const byte H = 2;

  int  x;
  char y;
  byte type;
  byte timer;

  inline void inactivate() { x = EXIST_THRESHOLD; }
  inline bool exist() const { return x > EXIST_THRESHOLD; }
  void initialize(const char y, const byte type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);

  private:
  static const byte ZIG_PERIOD = 96;
  static const byte TRI_PERIOD = 128;
};

struct Bullet {
  static const byte W = 1;
  static const byte H = 1;

  fixed x, y, vx, vy;
  byte type;
  
  inline char fieldX() const { return x >> 8; }
  inline char fieldY() const { return y >> 8; }
  inline void inactivate() { x = -(64 << 8); }
  inline bool exist() const { return x > -(64 << 8); }
  void initialize(const char x, const char y, const float radian, const byte type);
  void move(Context& context);
  void draw(Context& context);
  void onHit(Context& context);
};

struct Particle {
  char x, y;
  byte type;
  byte limit;

  inline void activate(const char x, const char y) {
    this->x = x;
    this->y = y;
  }
  inline void inactivate() { x = EXIST_THRESHOLD; }
  inline bool exist() const { return x > EXIST_THRESHOLD; }
  //void initialize(const char x, char float y, const byte type); // in order to reduce memory
  void move(Context& context);
  void draw(Context& context);
};

