#pragma once

#include "gamecore.h"
#include "constants.h"
#include "util.h"
#include "actor.h"
#include "echo.h"

struct Submarine;
struct Torpedo;

struct Context {
  GameCore& core;
  
  Context(GameCore& core) : core(core) {
  }
  
  inline long frameCount() const { return frames; }

  void initialize() {
    frames = 0;
    submarine.initialize();
    torpedo.initialize();
    inactivateCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    inactivateCharacters<Bullet>(bullets, BULLET_MAX);
    inactivateCharacters<Particle>(particles, PARTICLE_MAX);
    echo.reset(*this, 0);
  }
  bool loop() {
    spawn();
    moveAll();
    hitTest();
    drawAll();
    ++frames;

    // if return true then exit
    if(core.pressed(BTN_A) && core.pressed(BTN_B) && core.pressed(BTN_L)) {
      return true;  // A+B+Left to terminate
    }
    return submarine.extraLives < -1;
  }
  
  void spawn() {
    if(frameCount() % 120 == 0) {
      spawnBigEnemy((frameCount()/120 * 30 + 10) % SCREEN_HEIGHT);
    }
  }
  void moveAll() {
    submarine.move(*this);
    echo.reset(*this, submarine.x);
    torpedo.move(*this);
    moveCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    moveCharacters<Bullet>(bullets, BULLET_MAX);
    moveCharacters<Particle>(particles, PARTICLE_MAX);
  }
  void hitTest() {
    // BigEnemy vs Torpedo
    static const int GRAZE_RANGE = 8;
    for(int i = 0; i < BIG_ENEMY_MAX; ++i) {
      if(!torpedo.exist()) { break; }
      if(!bigEnemies[i].exist()) { continue; }
      if(
        Collision(bigEnemies[i].x, bigEnemies[i].y, bigEnemies[i].W, bigEnemies[i].H, 
        torpedo.x, torpedo.y - GRAZE_RANGE, torpedo.W, torpedo.H + GRAZE_RANGE*2)
        ) {
        if(
          Collision(bigEnemies[i].x, bigEnemies[i].y, bigEnemies[i].W, bigEnemies[i].H, 
          torpedo.x, torpedo.y, torpedo.W, torpedo.H)
          ) {
          bigEnemies[i].onHit(*this);
          torpedo.onHit();
        }
        else {
          bigEnemies[i].onGraze();
        }
      }
    }

    // Bullet vs Submarine
    for(int i = 0; i < BULLET_MAX; ++i) {
      if(!submarine.exist()) { break; } // todo armer
      if(!bullets[i].exist()) { continue; }
      if(Collision(bullets[i].x, bullets[i].y, bullets[i].W, bullets[i].H, submarine.x, submarine.y, submarine.W, submarine.H)) {
        bullets[i].onHit(*this);
        submarine.onHit();
      }
    }
  }
  void drawAll() {
    const int fieldX = -frameCount() / 16;
    DrawWave(core, fieldX, frameCount());
    DrawBottom(core, fieldX);
    
    echo.draw(*this);
    submarine.draw(*this);
    torpedo.draw(*this);
    drawCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    drawCharacters<Bullet>(bullets, BULLET_MAX);
    drawCharacters<Particle>(particles, PARTICLE_MAX);

    // disp extralives
    char text[2];
    sprintf(text, "%d", submarine.extraLives < 0 ? 0 : submarine.extraLives);
    core.setCursor(0, 0);
    core.print(text);
  }

  void addEcho(float left, float top, float bottom) {
    echo.add(left, top, bottom);
  }
  float getSubmarineAngle(const float bx, const float by) const {
    return atan2(submarine.y - by, submarine.x - bx);
  }

  // spawn characters
  bool tryLaunchTorpedo(float sx, float sy) {
    torpedo.tryLaunch(sx, sy);
  }
  void spawnBigEnemy(float y) {
    for(int i = 0; i < BIG_ENEMY_MAX; ++i) {
      if(!bigEnemies[i].exist()) {
        bigEnemies[i].initialize(y);
        return;
      }
    }
  }
  void fireBullet(float sx, float sy, float radian, unsigned char type) {
    for(int i = 0; i < BULLET_MAX; ++i) {
      if(!bullets[i].exist()) {
        bullets[i].initialize(sx, sy, radian, type);
        return;
      }
    }
  }
  void spawnParticle(float x, float y, unsigned char type) {
    for(int i = 0; i < PARTICLE_MAX; ++i) {
      if(!particles[i].exist()) {
        particles[i].initialize(x, y, type);
        return;
      }
    }
  }

  private:
  template<typename T> void inactivateCharacters(T pool[], const int n) {
    for(int i = 0; i < n; ++i) {
      pool[i].inactivate();
    }
  }
  template<typename T> void moveCharacters(T pool[], const int n) {
    for(int i = 0; i < n; ++i) {
      if(pool[i].exist()) { pool[i].move(*this); }
    }
  }
  template<typename T> void drawCharacters(T pool[], const int n) {
    for(int i = 0; i < n; ++i) {
      if(pool[i].exist()) { pool[i].draw(*this); }
    }
  }
  
  private:
  Echo echo;
  
  Submarine submarine;
  Torpedo torpedo;
  BigEnemy bigEnemies[BIG_ENEMY_MAX];
  Bullet bullets[BULLET_MAX];
  Particle particles[PARTICLE_MAX];

  long frames;
};
