#pragma once

#include "gamecore.h"
#include "constants.h"
#include "util.h"
#include "actor.h"
#include "echo.h"
#include "systembitmaps.h"

struct Submarine;
struct Torpedo;

struct Context {
  GameCore& core;
  
  Context(GameCore& core) : core(core) {
  }
  
  inline unsigned long frameCount() const { return frames; }
  inline void setGameover() {
    if(gameoverCount < 0) {
      gameoverCount = 0;
    }
  }
  inline bool isGameover() const { return gameoverCount >= 0; }

  void initialize() {
    frames = 0;
    gameoverCount = -1;
    
    submarine.initialize();
    torpedo.initialize();
    inactivateCharacters<AutoShot>(autoShots, AUTO_SHOT_MAX);
    inactivateCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    inactivateCharacters<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    inactivateCharacters<Bullet>(bullets, BULLET_MAX);
    inactivateCharacters<Particle>(particles, PARTICLE_MAX);
    
    echo.reset(*this, 0);
  }
  bool loop() {
    spawn();
    moveAll();
    hitTest();
    drawAll();

    // clock
    ++frames;
    if(gameoverCount >= 0) { ++gameoverCount; }

    // exit game if return true
    if(core.pressed(BTN_A) && core.pressed(BTN_B) && core.pressed(BTN_L)) {
      return true;  // A+B+Left to terminate
    }
    if(gameoverCount > 100 && (core.pressed(BTN_A) || core.pressed(BTN_B))) {
      return true;  // skip gameover text;
    }
    return gameoverCount >= 300;
  }
  
  void spawn() {
    if(frameCount() % 120 == 0) {
      spawnBigEnemy(random(8, SCREEN_HEIGHT-8));
      spawnSmallEnemy(random(8, SCREEN_HEIGHT-8), 0);
    }
  }
  void moveAll() {
    // to forecast the position of submarine
    prevSubmarineX = submarine.x;
    prevSubmarineY = submarine.y;
    
    submarine.move(*this);
    echo.reset(*this, submarine.x);
    torpedo.move(*this);
    moveCharacters<AutoShot>(autoShots, AUTO_SHOT_MAX);
    moveCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    moveCharacters<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    moveCharacters<Bullet>(bullets, BULLET_MAX);
    moveCharacters<Particle>(particles, PARTICLE_MAX);
  }
  void hitTest() {
    // BigEnemy vs Torpedo
    static const int GRAZE_RANGE = 8;
    for(int i = 0; i < BIG_ENEMY_MAX; ++i) {
      if(!torpedo.exist()) { break; }
      if(!bigEnemies[i].exist()) { continue; }
      // check graze
      if(Collision(
        bigEnemies[i].x, bigEnemies[i].y, bigEnemies[i].W, bigEnemies[i].H, 
        torpedo.x, torpedo.y - GRAZE_RANGE, torpedo.W, torpedo.H + GRAZE_RANGE*2
      )) {
        // check hit
        if(Collision(
          bigEnemies[i].x, bigEnemies[i].y, bigEnemies[i].W, bigEnemies[i].H, 
          torpedo.x, torpedo.y, torpedo.W, torpedo.H
        )) {
          bigEnemies[i].onHit(*this);
          torpedo.onHit();
        }
        else {
          bigEnemies[i].onGraze();
        }
      }
    }

    // SmallEnemy vs Torpedo
    for(int i = 0; i < SMALL_ENEMY_MAX; ++i) {
      if(!torpedo.exist()) { break; }
      if(!smallEnemies[i].exist()) { continue; }
      if(Collision(
        smallEnemies[i].x, smallEnemies[i].y, smallEnemies[i].W, smallEnemies[i].H,
        torpedo.x, torpedo.y - GRAZE_RANGE, torpedo.W, torpedo.H + GRAZE_RANGE*2
      )) {
        smallEnemies[i].onHit(*this);
      }
    }

    // AutoShot
    for(int shotIdx = 0; shotIdx < AUTO_SHOT_MAX; ++shotIdx) {
      if(!autoShots[shotIdx].exist()) { continue; }
      // vs SmallEnemy
      for(int smallEnemyIdx = 0; smallEnemyIdx < SMALL_ENEMY_MAX; ++smallEnemyIdx) {
        if(!smallEnemies[smallEnemyIdx].exist()) { continue; }
        if(Collision(
          autoShots[shotIdx].x, autoShots[shotIdx].y, autoShots[shotIdx].W, autoShots[shotIdx].H,
          smallEnemies[smallEnemyIdx].x, smallEnemies[smallEnemyIdx].y, smallEnemies[smallEnemyIdx].W, smallEnemies[smallEnemyIdx].H
        )) {
          autoShots[shotIdx].onHit();
          smallEnemies[smallEnemyIdx].onHit(*this);
        }
      }
      // vs BigEnemy
      for(int bigEnemyIdx = 0; bigEnemyIdx < BIG_ENEMY_MAX; ++bigEnemyIdx) {
        if(!bigEnemies[bigEnemyIdx].exist()) { continue; }
        if(Collision(
          autoShots[shotIdx].x, autoShots[shotIdx].y, autoShots[shotIdx].W, autoShots[shotIdx].H,
          bigEnemies[bigEnemyIdx].x, bigEnemies[bigEnemyIdx].y, bigEnemies[bigEnemyIdx].W, bigEnemies[bigEnemyIdx].H
        )) {
          autoShots[shotIdx].onHit();
        }
      }
    }

    // Bullet vs Submarine
    for(int i = 0; i < BULLET_MAX; ++i) {
      if(!submarine.exist()) { break; } // todo armer
      if(!bullets[i].exist()) { continue; }
      if(Collision(bullets[i].x, bullets[i].y, bullets[i].W, bullets[i].H, submarine.x, submarine.y, submarine.W, submarine.H)) {
        bullets[i].onHit(*this);
        submarine.onHit(*this);
      }
    }
  }
  void drawAll() {
    // background
    const int fieldX = -frameCount() / 16;
    DrawWave(core, fieldX, frameCount());
    DrawBottom(core, fieldX);

    // disp extralives
    core.drawBitmap(0, 4, bitmapExtraLives, 1);
    char text[2];
    sprintf(text, "%d", submarine.extraLives < 0 ? 0 : submarine.extraLives);
    core.setCursor(10, 0);
    core.print(text);
    
    // echo
    if(!isGameover()) {
      echo.draw(*this);
    }

    // characters
    drawCharacters<AutoShot>(autoShots, AUTO_SHOT_MAX);
    submarine.draw(*this);
    torpedo.draw(*this);
    drawCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    drawCharacters<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    drawCharacters<Bullet>(bullets, BULLET_MAX);
    drawCharacters<Particle>(particles, PARTICLE_MAX);

    // gameover
    if(gameoverCount > 100) {
      core.setCursor(SCREEN_WIDTH / 2 - 24, SCREEN_HEIGHT / 2 - 3);
      core.print("GAMEOVER");
    }
  }

  void addEcho(const float left, const float top, const float bottom) {
    echo.add(left, top, bottom);
  }
  float getSubmarineAngle(const float bx, const float by) const {
    return atan2(submarine.y - by, submarine.x - bx);
  }
  // FromX, FromY, BulletSpeed
  float getFutureSubmarineAngle(const float fx, const float fy, const float bs) const {
    const float tx = submarine.x;             // ToX (or TargetX)
    const float ty = submarine.y;             // ToY
    const float vx = tx - prevSubmarineX;     // VelocityX (of submarine)
    const float vy = ty - prevSubmarineY;     // VelocityY
    const float dr = atan2(ty - fy, tx - fx); // DiRection (to submarine)
    
    const float ds = (tx - fx) * (tx - fx) + (ty - fy) * (ty - fy); // SquareDistance
    const float rt = sqrt(ds) / bs;           // ReachTime
    
    const float px = vx * rt + tx;            // PredictedX (of future submarine)
    const float py = vy * rt + ty;            // PredictedY
    return atan2(py - fy, px - fx);
  }

  // spawn characters
  bool tryLaunchTorpedo(const float x, const float y) {
    torpedo.tryLaunch(x, y);
  }
  void fireAutoShot(const float x, const float y) {
    const int i = searchAvailableIndex<AutoShot>(autoShots, AUTO_SHOT_MAX);
    if(i >= 0) {
      autoShots[i].initialize(x, y);
    }
  }
  void spawnBigEnemy(const float y) {
    const int i = searchAvailableIndex<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    if(i >= 0) {
      bigEnemies[i].initialize(y);
    }
  }
  void spawnSmallEnemy(const float y, const unsigned char type) {
    const int i = searchAvailableIndex<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    if(i >= 0) {
      smallEnemies[i].initialize(y, type);
    }
  }
  void fireBullet(const float x, const float y, const float radian, const unsigned char type) {
    const int i = searchAvailableIndex<Bullet>(bullets, BULLET_MAX);
    if(i >= 0) {
      bullets[i].initialize(x, y, radian, type);
    }
  }
  void spawnParticle(const float x, const float y, const unsigned char type) {
    const int i = searchAvailableIndex<Particle>(particles, PARTICLE_MAX);
    if(i >= 0) {
      particles[i].initialize(x, y, type);
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
  template<typename T> int searchAvailableIndex(const T pool[], const int n) {
    for(int i = 0; i < n; ++i) {
      if(!pool[i].exist()) { return i; }
    }
    return -1;  // not found
  }
  
  private:
  Echo echo;
  
  Submarine  submarine;
  Torpedo    torpedo;

  AutoShot   autoShots[AUTO_SHOT_MAX];
  BigEnemy   bigEnemies[BIG_ENEMY_MAX];
  SmallEnemy smallEnemies[SMALL_ENEMY_MAX];
  Bullet     bullets[BULLET_MAX];
  Particle   particles[PARTICLE_MAX];

  unsigned long frames;
  float prevSubmarineX, prevSubmarineY;
  int   gameoverCount;
};

