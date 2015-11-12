#pragma once

#include "gamecore.h"
#include "constants.h"
#include "util.h"
#include "actor.h"
#include "modules.h"
#include "assets.h"

struct Submarine;
struct Torpedo;

struct GameLevel {
  GameCore& core;
  Echo      echo;
  Platoons  platoons;
  
  GameLevel(GameCore& core) : core(core) {
  }
  
  inline unsigned long frameCount() const { return frames; }
  inline void setGameover() {
    if(gameoverCount < 0) {
      gameoverCount = 0;
    }
  }
  inline bool isGameover() const { return gameoverCount >= 0; }
  inline unsigned int getScore() const { return score; }
  inline byte difficulty() const { return generator.getDifficulty(); }
#ifndef LOW_FLASH_MEMORY
  bool lookForEnemy() const {
    for(byte i = 0; i < BIG_ENEMY_MAX; ++i) {
      if(bigEnemies[i].x > 0 && bigEnemies[i].x < SCREEN_WIDTH + 16) {
        return true;
      }
    }
    for(byte i = 0; i < SMALL_ENEMY_MAX; ++i) {
      if(smallEnemies[i].x > 0 && smallEnemies[i].x < SCREEN_WIDTH + 16) {
        return true;
      }
    }
    return false;
  }
  void removeAllBullets() {
    for(byte i = 0; i < BULLET_MAX; ++i) {
      bullets[i].onHit(*this);
    }
  }
#endif

  void onEntry() {
    frames = 0;
    score  = 0;
    gameoverCount = -1;
    
    submarine.initialize();
    torpedo.inactivate();
    inactivateCharacters<AutoShot>(autoShots, AUTO_SHOT_MAX);
    inactivateCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    //inactivateCharacters<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    inactivateCharacters<Bullet>(bullets, BULLET_MAX);
    inactivateCharacters<Particle>(particles, PARTICLE_MAX);

    // first shoal
    for(byte i = 0; i < SMALL_ENEMY_MAX; ++i) {
      const byte r = random(256);
      smallEnemies[i].x = FIELD_WIDTH - r % SCREEN_WIDTH;
      smallEnemies[i].y = r % (SCREEN_HEIGHT - 20) + 10;
      smallEnemies[i].type  = SENEMY_ZIG_NOFIRE;
      smallEnemies[i].timer = r % 96;
    }
    //inactivateCharacters<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    
    echo.reset(*this, 0);
    platoons.initialize();
    generator.initialize();

    randomSeed(core.frameCount());
    core.tone(880, 1000);
  }
  bool loop() {
    // music
#ifndef DISABLE_MUSIC
    if(frames > 381 && !core.playing()) { core.playScore(music); }
#endif
    
    // spawn
    /*if(frameCount() % 240 == 0) {
      //spawnBigEnemy(random(8, SCREEN_HEIGHT-8));
      //spawnSmallEnemy(random(8, SCREEN_HEIGHT-8), SENEMY_ZIG_NOFIRE | (3 << 4));
      platoons.set(random(8, SCREEN_HEIGHT - 8), PLATOON_TRI_SHOAL);
    }*/
    generator.spawn(*this);
    platoons.spawn(*this);

    // in order to forecast the position of submarine
    prevSubmarineX = submarine.x * (1.f/256);
    prevSubmarineY = submarine.y * (1.f/256);
    
    // === move ===
    submarine.move(*this);
    echo.reset(*this, submarine.fieldX());
    torpedo.move(*this);
    moveCharacters<AutoShot>(autoShots, AUTO_SHOT_MAX);
    moveCharacters<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    moveCharacters<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    moveCharacters<Bullet>(bullets, BULLET_MAX);
    moveCharacters<Particle>(particles, PARTICLE_MAX);

    // === hittest ===
    // BigEnemy vs Torpedo
    static const byte GRAZE_RANGE = 8;
    for(byte i = 0; i < BIG_ENEMY_MAX; ++i) {
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
#ifndef LOW_FLASH_MEMORY
    // SmallEnemy vs Torpedo
    for(byte i = 0; i < SMALL_ENEMY_MAX; ++i) {
      if(!torpedo.exist()) { break; }
      if(!smallEnemies[i].exist()) { continue; }
      if(Collision(
        smallEnemies[i].x, smallEnemies[i].y, smallEnemies[i].W, smallEnemies[i].H,
        torpedo.x, torpedo.y, torpedo.W, torpedo.H
      )) {
        smallEnemies[i].onHit(*this);
      }
    }
#endif
    // AutoShot
    for(byte shotIdx = 0; shotIdx < AUTO_SHOT_MAX; ++shotIdx) {
      if(!autoShots[shotIdx].exist()) { continue; }
      // vs SmallEnemy
      for(byte smallEnemyIdx = 0; smallEnemyIdx < SMALL_ENEMY_MAX; ++smallEnemyIdx) {
        if(!smallEnemies[smallEnemyIdx].exist()) { continue; }
        if(Collision(
          autoShots[shotIdx].x, autoShots[shotIdx].y, autoShots[shotIdx].W, autoShots[shotIdx].H,
          smallEnemies[smallEnemyIdx].x, smallEnemies[smallEnemyIdx].y, smallEnemies[smallEnemyIdx].W, smallEnemies[smallEnemyIdx].H
        )) {
          autoShots[shotIdx].onHit();
          smallEnemies[smallEnemyIdx].onHit(*this);
        }
      }
#ifndef LOW_FLASH_MEMORY
      // vs BigEnemy
      for(byte bigEnemyIdx = 0; bigEnemyIdx < BIG_ENEMY_MAX; ++bigEnemyIdx) {
        if(!bigEnemies[bigEnemyIdx].exist()) { continue; }
        if(Collision(
          autoShots[shotIdx].x, autoShots[shotIdx].y, autoShots[shotIdx].W, autoShots[shotIdx].H,
          bigEnemies[bigEnemyIdx].x, bigEnemies[bigEnemyIdx].y, bigEnemies[bigEnemyIdx].W, bigEnemies[bigEnemyIdx].H
        )) {
          autoShots[shotIdx].onHit();
        }
      }
#endif
    }

    // Bullet vs Submarine
    for(byte i = 0; i < BULLET_MAX; ++i) {
      if(!submarine.exist()) { break; } // todo armer
      if(!bullets[i].exist()) { continue; }
      if(Collision(
        bullets[i].fieldX(), bullets[i].fieldY(), bullets[i].W, bullets[i].H, 
        submarine.fieldX(), submarine.fieldY(), submarine.W, submarine.H
      )) {
        bullets[i].onHit(*this);
        submarine.onHit(*this);
      }
    }

    // === draw ===
    // background
    const int fx = -frameCount() / 16;
    DrawWave(core, fx, frameCount());
    DrawBottom(core, fx);

    // disp extralives
    core.drawBitmap(0, 4, bitmapExtraLives, 1);
    char text[6];
    sprintf(text, "%c", '0' + (submarine.extraLives < 0 ? 0 : submarine.extraLives));
    core.setCursor(10, 0);
    core.print(text);

#ifdef DEBUG
    // disp score
    sprintf(text, "%05d", score);
    core.setCursor(SCREEN_WIDTH/2 - 15, 0);
    core.print(text);
#endif

    // zone and score
    if(!isGameover()) {
      generator.draw(*this);
    }
    
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

    // === post process ===
    // clock
    ++frames;
    if(gameoverCount >= 0) { ++gameoverCount; }

    // exit game if return true
    if(gameoverCount > 100 && (core.pressed(BTN_A) || core.pressed(BTN_B))) {
      return true;  // skip gameover text;
    }
    return gameoverCount >= 300;
  }

  void addScore(unsigned int plus) {
    if(score + plus < score) {
      score = 0xffff; // count stop
    }
    else if(gameoverCount < 0) {
#ifndef EXHIBITION_MODE
      if((score + plus) / EXTEND_SCORE > score / EXTEND_SCORE && submarine.extraLives < 127) {
        ++submarine.extraLives;
        core.tone(1760, 500);
      }
#endif
      score += plus;
    }
  }
  float getSubmarineAngle(const char bx, const char by) const {
    return atan2(submarine.fieldY() - by, submarine.fieldX() - bx);
  }
  // FromX, FromY, BulletSpeed
  float getFutureSubmarineAngle(const char fx, const char fy, const char bs) const {
    const float tx = submarine.x * (1.f/256);  // ToX (or TargetX)
    const float ty = submarine.y * (1.f/256);  // ToY
    const float vx = tx - prevSubmarineX;      // VelocityX (of submarine)
    const float vy = ty - prevSubmarineY;      // VelocityY
    const float dr = atan2(ty - fy, tx - fx);  // DiRection (to submarine)
    
    const float ds = (tx - fx) * (tx - fx) + (ty - fy) * (ty - fy); // SquareDistance
    const float rt = sqrt(ds) / bs;            // ReachTime
    
    const float px = vx * rt + tx;             // PredictedX (of future submarine)
    const float py = vy * rt + ty;             // PredictedY
    return atan2(py - fy, px - fx);
  }

  // spawn characters
  void launchTorpedo(const char x, const char y) {
    if(torpedo.launch(x, y)) {
      core.tone(440, 50);
    }
  }
  void fireAutoShot(const char x, const char y) {
    const char i = searchAvailableIndex<AutoShot>(autoShots, AUTO_SHOT_MAX);
    if(i >= 0) {
      autoShots[i].initialize(x, y);
    }
  }
  void spawnBigEnemy(const char y) {
    const char i = searchAvailableIndex<BigEnemy>(bigEnemies, BIG_ENEMY_MAX);
    if(i >= 0) {
      bigEnemies[i].initialize(y);
    }
  }
  bool spawnSmallEnemy(const char y, const byte type) {
    const char i = searchAvailableIndex<SmallEnemy>(smallEnemies, SMALL_ENEMY_MAX);
    if(i >= 0) {
      smallEnemies[i].initialize(y, type);
      return false;  // successfully
    }
    return true; // error
  }
  void fireBullet(const char x, const char y, const float radian, const byte type) {
    const char i = searchAvailableIndex<Bullet>(bullets, BULLET_MAX);
    if(i >= 0) {
      bullets[i].initialize(x, y, radian, type);
    }
  }
  void spawnParticle(const int x, const char y, const byte type) {
    // this function can not be inline
    if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) { return; }
    const char i = searchAvailableIndex<Particle>(particles, PARTICLE_MAX);
    if(i >= 0) {
      particles[i].activate(x, y);
      particles[i].type = type;
      switch(type) {
        case PARTICLE_EXPLOSION: particles[i].limit = 12; break;
        default:                 particles[i].limit = 50; break;
      }
    }
  }

  private:
  template<typename T> inline void inactivateCharacters(T pool[], const byte n) {
    for(byte i = 0; i < n; ++i) {
      pool[i].inactivate();
    }
  }
  template<typename T> inline void moveCharacters(T pool[], const byte n) {
    for(byte i = 0; i < n; ++i) {
      if(pool[i].exist()) { pool[i].move(*this); }
    }
  }
  template<typename T> inline void drawCharacters(T pool[], const byte n) {
    for(byte i = 0; i < n; ++i) {
      if(pool[i].exist()) { pool[i].draw(*this); }
    }
  }
  template<typename T> inline char searchAvailableIndex(const T pool[], const byte n) const {
    for(byte i = 0; i < n; ++i) {
      if(!pool[i].exist()) { return i; }
    }
    return -1;  // not found
  }
  
  private:
  Generator  generator;
  
  Submarine  submarine;
  Torpedo    torpedo;

  AutoShot   autoShots[AUTO_SHOT_MAX];
  BigEnemy   bigEnemies[BIG_ENEMY_MAX];
  SmallEnemy smallEnemies[SMALL_ENEMY_MAX];
  Bullet     bullets[BULLET_MAX];
  Particle   particles[PARTICLE_MAX];

  unsigned long frames;
  unsigned int  score;
  float prevSubmarineX, prevSubmarineY;
  int   gameoverCount;
};

