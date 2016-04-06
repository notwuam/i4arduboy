#include "actor.h"
#include "gamelevel.h"
#include "modules.h"
#include "util.h"
#include "assets.h"

// === Submarine ===

void Submarine::initialize() {
  x = W << 8;
  y = (SCREEN_HEIGHT / 2) << 8;
  extraLives = START_LIVES;
  armor      = ARMOR_FRAMES;
}

void Submarine::move(GameLevel& context) {
  if(context.isGameover()) { return; }
  
  // control
  static const int SPD = 1 << 7;
  static const int R2  = 90;  // 0.5/sqrt(2) / (1/256)
  if(context.core.pressed(BTN_U) && context.core.pressed(BTN_L)) {
    x -= R2;
    y -= R2;
  }
  else if(context.core.pressed(BTN_U) && context.core.pressed(BTN_R)) {
    x += R2;
    y -= R2;
  }
  else if(context.core.pressed(BTN_D) && context.core.pressed(BTN_L)) {
    x -= R2;
    y += R2;
  }
  else if(context.core.pressed(BTN_D) && context.core.pressed(BTN_R)) {
    x += R2;
    y += R2;
  }
  else if(context.core.pressed(BTN_U)) {
    y -= SPD;
  }
  else if(context.core.pressed(BTN_L)) {
    x -= SPD;
  }
  else if(context.core.pressed(BTN_R)) {
    x += SPD;
  }
  else if(context.core.pressed(BTN_D)) {
    y += SPD;
  }

  // clamping into field
  static const fixed MARGIN = 6;
  x = Clamp(x, 0, (SCREEN_WIDTH - MARGIN) << 8);
  y = Clamp(y, (MARGIN - H) << 8, (SCREEN_HEIGHT - MARGIN) << 8);

  // launching torpedo
  if(extraLives >= 0 && (context.core.pushed(BTN_A) || context.core.pushed(BTN_B))) {
    context.launchTorpedo(fieldX() + 10, fieldY() + 1);
  }

  //firing auto shot
#ifdef LOW_FLASH_MEMORY
  if(context.frameCount() % 5 == 0) {
    context.fireAutoShot(fieldX() + 3, fieldY() - 3);
  }
#else
  if(context.lookForEnemy() && context.frameCount() % 5 == 0) {
    context.fireAutoShot(fieldX() + 3, fieldY() - 3);
  }
#endif

  // updating armor timer
  if(armor > 0) { --armor; }
}

void Submarine::draw(GameLevel& context) const {
  if(armor == 0 || (extraLives >= 0 && context.frameCount() / 3 % 2 == 0)) {
    context.core.drawBitmap(fieldX() - 1, fieldY() - 2, bitmapSubmarine, 2);
    if(context.frameCount() % ECHO_CYCLE > 30) {  // sustain: 30 frames
      context.core.drawPixel(fieldX() + 6, fieldY() - 1, 1);
    }
  }
  //context.getArduboy().drawRect(x, y, W, H, 0);
}

void Submarine::onHit(GameLevel& context) {
  if(armor <= 0) {
    if(extraLives >= 0) {
      context.spawnParticle(fieldX() - 2, fieldY() - 4, PARTICLE_EXPLOSION);
      context.core.tone(185, 250);
    }
    --extraLives;
    if(extraLives < 0) {
      context.setGameover();
      // ToDo: add gameover sfx
      inactivate();
    }
    armor = ARMOR_FRAMES;
  }
}


// === Torpedo ===

bool Torpedo::launch(const char x, const char y) {
  if(!exist()) {
    this->x = x;
    this->y = y;
    vx = 0;
    return true;
  }
  return false;
}

void Torpedo::move() {
  static const fixed ACC = 1 << 7;  // 0.5
  if(!exist()) { return; }
  vx += ACC - (vx >> 4);
  x  += vx >> 8;
  if(x >= FIELD_WIDTH) {
    inactivate();
  }
}

void Torpedo::draw(GameLevel& context) const {
  if(!exist()) { return; }
  // body
  context.core.drawBitmap(x, y - 1, bitmapTorpedo, 2);
  // shockwave
  if(vx > 4 << 8) { // well accelerated
    const byte h = bitmapShockwave0[1];
    context.core.drawBitmap(x -  8, y - h/2, bitmapShockwave0, 2);
    context.core.drawBitmap(x - 16, y - h/2, bitmapShockwave1, 2);
    context.core.drawBitmap(x - 24, y - h/2, bitmapShockwave2, 2);
  }
  //context.getArduboy.drawFastHLine(x, y, W, 0);
}


// === AutoShot ===

void AutoShot::initialize(const char x, const char y) {
  this->x = x;
  this->y = y;
}

void AutoShot::move() {
  // moving
  x += 8;

  // frame out
  if(x > SCREEN_WIDTH) {
    inactivate();
  }
}

void AutoShot::draw(GameLevel& context) const {
  context.core.drawBitmap((int)x + 2, (int)y + 3, bitmapAutoShot, 2);
  //context.core.getArduboy().drawRect(x, y, W, H, 1);
}


// === BigEnemy ===

void BigEnemy::initialize(const char y) {
  this->x = FIELD_WIDTH;
  this->y = y;
  state   = 0;
  timer   = 0;
}

void BigEnemy::move(GameLevel& context) {
  // moving
  if(grazed() && x > SCREEN_WIDTH - 5) {
    x += -3; // submarine found
  }
  else if(timer % 5 == 0) { // -0.2
    --x;
  }

  // frame out
  if(x + bitmapCruEnemy0[0] < 0) {
    inactivate();
  }

  // setting sonar reaction
  context.echo.add(x, y, y+H);

  // firing bullet
  if(x < SCREEN_WIDTH - W / 2) {
    if(!onScreen()) {
      timer = 0;  // in order to sync fire cycle
      state |= ON_SCREEN_MASK;
    }
    const byte d = context.difficulty();
    if(timer == 0 || (d >= 60 && timer == 40)) {  // freq up when 60 and over
      const char bulletY     = y + bitmapCruEnemy0[1] / 2;
      const char bulletSpeed = (d >= 80) ? BULLET_TYPE2_SPD : BULLET_TYPE0_SPD;
      const char bulletType  = (d >= 80) ? 2 : 0;
      context.fireBullet(x, bulletY, context.getFutureSubmarineAngle(x, bulletY, bulletSpeed), bulletType);
    }
  }

  // updating timer
  static const byte PERIOD = 150;
  timer = (timer + 1) % PERIOD;
}

void BigEnemy::draw(GameLevel& context) const {
  if(x - 3 > SCREEN_WIDTH) { return; }
  context.core.drawBitmap(x - 3, y - 2, bitmapCruEnemy0, 2);
}

void BigEnemy::onHit(GameLevel& context) {
  // add score
  context.addScore(10);
  if(x > SCREEN_WIDTH) {  // far bonus
    context.addScore((x - SCREEN_WIDTH) * 10 / (FIELD_WIDTH - SCREEN_WIDTH));
  }
  // near
  if(x < SCREEN_WIDTH + 20) {
    context.core.tone(1047, 250);
    context.spawnParticle(x + random(-2,  8), y + random(-4, 4), PARTICLE_EXPLOSION);
    context.spawnParticle(x + random( 8, 18), y + random(-4, 4), PARTICLE_EXPLOSION);
#ifndef LOW_FLASH_MEMORY
    context.removeAllBullets();
    context.core.setQuake();
#endif
  }
  // reset
  inactivate();
}


// === SmallEnemy ===

void SmallEnemy::initialize(const char y, const byte type) {
  this->x    = FIELD_WIDTH;
  this->y    = y;
  this->type = type;
  timer      = 0;
}

void SmallEnemy::move(GameLevel& context) {
  const byte period = (getType() == 0) ? ZIG_PERIOD : TRI_PERIOD;

  switch(getType()) {
    // zigzag
    case 0: {
      // moving
      // x -= 1.5, y -= 0.5
      if(timer % 2 == 0) {
        x -= 2;
        y += (timer / (ZIG_PERIOD / 4) % 2 == 0) ? 1 : -1;
      }
      else {
        --x;
      }
      
      // firing bullet
      const byte d = context.difficulty();
      const byte typeCond = canFire() || 
        (d >= 70 && d < 80) || d >= 100;  // [70,80) or 100
      if(
        timer == 80 && typeCond &&    // time / type
        x < SCREEN_WIDTH - W / 2 &&   // position
        d >= 20                       // dont fire less than 20
      ) {
        const byte  bulletType = (d >= 80) ? 3 : 1;  // rapid when 80 and over
        const float subAngle   = context.getSubmarineAngle(x, y);
        context.fireBullet(x, y, subAngle, bulletType);
        // 3way
        if((d >= 50 && d < 70) || d >= 90) {
          context.fireBullet(x, y, subAngle - radians(10), 1);
          context.fireBullet(x, y, subAngle + radians(10), 1);
        }
      }
    } break;

    // triangle
    default: {
      // moving
      if(timer % 6 == 0) { --x; }
      const int half = period / 2;
      int tmp = half - timer % half;
      tmp = tmp*tmp / (half*half/4) - 1;
      if(tmp < 0) { tmp = 0; }
      x -= tmp;
      
      // firing bullet
      const bool timeCond = timer == 64 || 
        (context.difficulty() >= 25 && timer == 32) || 
        (context.difficulty() >= 75 && timer == 96);
      if(
        timeCond && canFire() &&                    // time / type
        x < SCREEN_WIDTH && x > SCREEN_WIDTH / 2 && // position
        context.difficulty() >= 20                  // difficulty
      ) {
        context.fireBullet(x, y, context.getSubmarineAngle(x, y), 1);
      }
    } break;
  }

  // frame out
  if(x + 12 < 0) {
    context.platoons.checkBonus(getPlatoon(), false);
    inactivate();
  }

  // setting sonar reaction
  context.echo.add(x, y, y+H);

  // updating timer
  timer = (timer + 1) % period;
}

void SmallEnemy::draw(GameLevel& context) const {
  if(x - 4 > SCREEN_WIDTH) { return; }
  switch(getType()) {
    // zigzag
    case 0: {
      const byte* bitmaps[] = {bitmapZigEnemy0, bitmapZigEnemy1};
      const byte frame = timer / (ZIG_PERIOD / 4) % 2;
      context.core.drawBitmap(x - 3, y - 3, bitmaps[frame], 2);
    } break;
    
    // triangle
    default: {
      const byte* bitmaps[] = {bitmapTriEnemy0, bitmapTriEnemy1};
      const byte frame = (timer % (TRI_PERIOD / 2) < 24) ? 1 : 0;
      context.core.drawBitmap(x - 3, y - 4, bitmaps[frame], 2);
      if(frame == 1) {
        context.core.drawBitmap(x + 7, y - 2, bitmapBoostEffect, 2);
      }
    } break;
  }
  //context.core.getArduboy().drawRect(x, y, W, H, 0);
}

void SmallEnemy::onHit(GameLevel& context) {
  context.addScore(1);
  if(x < SCREEN_WIDTH + 20) { // near
    context.spawnParticle(round(x) - 5, round(y) - 4, PARTICLE_EXPLOSION);
    context.core.tone(880, 62);
  }
  // platoon elimination
  if(context.platoons.checkBonus(getPlatoon(), true)) {
    if(x < SCREEN_WIDTH + 20) { // near
      context.spawnParticle(round(x) - 2, round(y) - 2, PARTICLE_TEN_POINT);
    }
    context.addScore(10);
  }
  inactivate();
}


// === Bullet ===

void Bullet::initialize(const char x, const char y, const float radian, const byte type) {
  this->x    = x << 8;
  this->y    = y << 8;
  this->type = type;

  const byte SPEEDS[] = {BULLET_TYPE0_SPD, BULLET_TYPE1_SPD, BULLET_TYPE2_SPD, BULLET_TYPE3_SPD};
  byte speed = SPEEDS[type];
  vx = round(cos(radian) * speed / (1.f / 256));
  vy = round(sin(radian) * speed / (1.f / 256));
}

void Bullet::move(GameLevel& context) {
  x += vx;
  y += vy;

  // frame out
  static const char MARGIN = 4;
  if(
    fieldX() < -MARGIN || fieldX() > SCREEN_WIDTH  + MARGIN ||
    fieldY() < -MARGIN || fieldY() > SCREEN_HEIGHT + MARGIN
  ) {
    inactivate();  
  }
}

void Bullet::draw(GameLevel& context) const {
  // ToDo: async animation (if there are enough memories)
  const byte frame = context.frameCount() / 3 % 2;
  
  if(type % 2 == 0) {
    const byte* bitmaps[] = {bitmapMbullet0, bitmapMbullet1};
    context.core.drawBitmap(fieldX() - bitmapMbullet0[0]/2, fieldY() - bitmapMbullet0[1]/2, bitmaps[frame], 2);
  }
  else {
    const byte* bitmaps[] = {bitmapSbullet0, bitmapSbullet1};
    context.core.drawBitmap(fieldX() - bitmapSbullet0[0]/2, fieldY() - bitmapSbullet0[1]/2, bitmaps[frame], 2);
  }
  //arduboy.drawPixel(x, y, 0);
}

void Bullet::onHit(GameLevel& context) {
  context.core.drawCircle(fieldX(), fieldY(), 6, 1);
  inactivate();
}


// === Particle ===

void Particle::move(GameLevel& context) {
  --limit;
  if(limit <= 0) {
    inactivate();
  }
}

void Particle::draw(GameLevel& context) const {
  switch(type) {
    case 0: {
      if(limit > 8) {
        context.core.drawBitmap(x, y, bitmapExplosion0, 2);
      }
      else if(limit > 4) {
        context.core.drawBitmap(x, y, bitmapExplosion1, 2);
      }
      else {
        context.core.drawBitmap(x, y, bitmapExplosion2, 2);
      }
    } break;
    
    default: {
      context.core.drawBitmap(x, y, bitmapTen, 2);
    } break;
  }
}

