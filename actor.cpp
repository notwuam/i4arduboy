#include "actor.h"
#include "context.h"
#include "modules.h"
#include "util.h"
#include "actorbitmaps.h"
#include "scores.h"

// === Submarine ===

void Submarine::initialize() {
  x = W << 8;
  y = (SCREEN_HEIGHT / 2) << 8;
  extraLives = START_LIVES;
  armer      = ARMER_FRAMES;
}

void Submarine::move(Context& context) {
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
  static const fixed MARGIN = 6 << 8;
  x = Clamp(x, 0, (SCREEN_WIDTH << 8) - MARGIN);
  y = Clamp(y, MARGIN - (H << 8), (SCREEN_HEIGHT << 8) - MARGIN);

  // launching torpedo
  if(extraLives >= 0 && (context.core.pushed(BTN_A) || context.core.pushed(BTN_B))) {
    context.launchTorpedo(fieldX() + 10, fieldY() + 1);
  }

  //firing auto shot
#ifdef LOW_MEMORY
  if(context.frameCount() % 5 == 0) {
    context.fireAutoShot(fieldX() + 3, fieldY() - 3);
  }
#else
  if(context.lookForEnemy() && context.frameCount() % 5 == 0) {
    context.fireAutoShot(fieldX() + 3, fieldY() - 3);
  }
#endif

  // updating armer timer
  if(armer > 0) { --armer; }
}

void Submarine::draw(Context& context) {
  if(armer == 0 || (extraLives >= 0 && context.frameCount() / 3 % 2 == 0)) {
    context.core.drawBitmap(fieldX() - 1, fieldY() - 2, bitmapSubmarine, 2);
    if(context.frameCount() % ECHO_CYCLE > 30) {  // sustain: 30 frames
      context.core.drawPixel(fieldX() + 6, fieldY() - 1, 1);
    }
  }
  //context.getArduboy().drawRect(x, y, W, H, 0);
}

void Submarine::onHit(Context& context) {
  if(armer <= 0) {
    if(extraLives >= 0) {
      context.spawnParticle(fieldX() - 2, fieldY() - 4, 0);
      context.core.tone(523, 250);
    }
    --extraLives;
    if(extraLives < 0) {
      context.setGameover();
      inactivate();
    }
    armer = ARMER_FRAMES;
  }
}


// === Torpedo ===

void Torpedo::launch(const char x, const char y) {
  if(!exist()) {
    this->x = x;
    this->y = y;
    vx = 0;
  }
}

void Torpedo::move(Context& context) {
  static const fixed ACC = 1 << 7;  // 0.5
  if(!exist()) { return; }
  vx += ACC - (vx >> 4);
  x  += vx >> 8;
  if(x >= FIELD_WIDTH) {
    inactivate();
  }
}

void Torpedo::draw(Context& context) {
  if(!exist()) { return; }
  // body
  context.core.drawBitmap(x, y - 1, bitmapTorpedo, 2);
  // shockwave
  if(vx > 4 << 8) { // well accelerated
    const byte w = bitmapShockwave0[0];
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

void AutoShot::move(Context& context) {
  // moving
  x += 8;

  // frame out
  if(x > SCREEN_WIDTH) {
    inactivate();
  }
}

void AutoShot::draw(Context& context) {
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

void BigEnemy::move(Context& context) {
  // moving
  static const float NORMAL_SPD = -0.2f;
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
    if(timer == 0) {
      const char by = y + bitmapCruEnemy0[1] / 2;
      context.fireBullet(x, by, context.getFutureSubmarineAngle(x, by, BULLET_TYPE0_SPD), 0);
    }
  }

  // updating timer
  static const byte PERIOD = 150;
  timer = (timer + 1) % PERIOD;
}

void BigEnemy::draw(Context& context) {
  if(x - 3 > SCREEN_WIDTH) { return; }
  context.core.drawBitmap(x - 3, y - 2, bitmapCruEnemy0, 2);
}

void BigEnemy::onHit(Context& context) {
  // add score
  context.addScore(10);
  if(x > SCREEN_WIDTH) {  // far bonus
    context.addScore((x - SCREEN_WIDTH) * 10 / (FIELD_WIDTH - SCREEN_WIDTH));
  }
  // near
#ifndef LOW_MEMORY
  if(x < SCREEN_WIDTH + 20) {
    context.removeAllBullets();
    context.core.setQuake();
    context.core.playScore(bing); // ToDo: another sfx
    context.spawnParticle(x + random(-2,  8), y + random(-4, 4), 0);
    context.spawnParticle(x + random( 8, 18), y + random(-4, 4), 0);
  }
#endif
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

void SmallEnemy::move(Context& context) {
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
      if(timer == 80 && canFire() && x < SCREEN_WIDTH - W / 2) {
        context.fireBullet(x, y, context.getSubmarineAngle(x, y), 1);
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
      if(timer == 64 && canFire() && x < SCREEN_WIDTH && x > SCREEN_WIDTH / 2) {
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

void SmallEnemy::draw(Context& context) {
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

void SmallEnemy::onHit(Context& context) {
  context.spawnParticle(round(x) - 5, round(y) - 4, 0);
  context.addScore(1);
  if(context.platoons.checkBonus(getPlatoon(), true)) {
    context.addScore(10);
  }
  inactivate();
}


// === Bullet ===

void Bullet::initialize(const char x, const char y, const float radian, const byte type) {
  this->x    = x << 8;
  this->y    = y << 8;
  this->type = type;
  
  const int speed = (type == 0 ? BULLET_TYPE0_SPD : BULLET_TYPE1_SPD);
  vx = round(cos(radian) * speed / (1.f / 256));
  vy = round(sin(radian) * speed / (1.f / 256));
}

void Bullet::move(Context& context) {
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

void Bullet::draw(Context& context) {
  // ToDo: async animation (if there are enough memories)
  const byte frame = context.frameCount() / 3 % 2;
  
  if(type == 0) {
    const byte* bitmaps[] = {bitmapMbullet0, bitmapMbullet1};
    context.core.drawBitmap(fieldX() - bitmapMbullet0[0]/2, fieldY() - bitmapMbullet0[1]/2, bitmaps[frame], 2);
  }
  else {
    const byte* bitmaps[] = {bitmapSbullet0, bitmapSbullet1};
    context.core.drawBitmap(fieldX() - bitmapSbullet0[0]/2, fieldY() - bitmapSbullet0[1]/2, bitmaps[frame], 2);
  }
  //arduboy.drawPixel(x, y, 0);
}

void Bullet::onHit(Context& context) {
  context.core.drawCircle(fieldX(), fieldY(), 6, 1);
  inactivate();
}


// === Particle ===

void Particle::move(Context& context) {
  --limit;
  if(limit <= 0) {
    inactivate();
  }
}

void Particle::draw(Context& context) {
//  switch(type) {
//    default: {
      if(limit > 8) {
        context.core.drawBitmap(x, y, bitmapExplosion0, 2);
      }
      else if(limit > 4) {
        context.core.drawBitmap(x, y, bitmapExplosion1, 2);
      }
      else {
        context.core.drawBitmap(x, y, bitmapExplosion2, 2);
      }
//    }
//  }
}

