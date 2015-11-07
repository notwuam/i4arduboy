#include "actor.h"
#include "context.h"
#include "echo.h"
#include "util.h"
#include "actorbitmaps.h"
#include "scores.h"

// === Submarine ===

void Submarine::initialize() {
  x = W;
  x <<= 8;
  y = SCREEN_HEIGHT / 2;
  y <<= 8;
  prevFire   = true;  // to prevent from launching a torpedo
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
  static const int MARGIN = 6 << 8;
  if(x < MARGIN) { x = MARGIN; }
  if(x > (SCREEN_WIDTH << 8) - MARGIN) { x = (SCREEN_WIDTH << 8) - MARGIN; }
  if(y < MARGIN) { y = MARGIN; }
  if(y > (SCREEN_HEIGHT << 8) - MARGIN) { y = (SCREEN_HEIGHT << 8) - MARGIN; }

  // launching torpedo
  if(extraLives >= 0 && (context.core.pressed(BTN_A) || context.core.pressed(BTN_B))) {
    if(!prevFire) {
      context.launchTorpedo(fieldX() + 10, fieldY() + 1);
    }
    prevFire = true;
  }
  else {
    prevFire = false;
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
      context.spawnParticle(x - 2, y - 4, 0);
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

void Torpedo::launch(const float x, const float y) {
  if(!exist()) {
    activate(x, y);
    vx = 0.f;
  }
}

void Torpedo::move(Context& context) {
  static const float ACC = 0.5f;
  static const float RSS = 0.1f;
  
  if(!exist()) { return; }
  vx += ACC - RSS * vx;
  x  += vx;
  if(x >= FIELD_WIDTH) {
    inactivate();
  }
}

void Torpedo::draw(Context& context) {
  if(!exist()) { return; }
  // body
  context.core.drawBitmap(x, y - 1, bitmapTorpedo, 2);
  // shockwave
  if(vx > 3.2f) { // well accelerated
    const byte w = bitmapShockwave0[0];
    const byte h = bitmapShockwave0[1];
    context.core.drawBitmap(x -  8, y - h/2, bitmapShockwave0, 2);
    context.core.drawBitmap(x - 16, y - h/2, bitmapShockwave1, 2);
    context.core.drawBitmap(x - 24, y - h/2, bitmapShockwave2, 2);
  }
  //context.getArduboy.drawFastHLine(x, y, W, 0);
}


// === AutoShot ===

void AutoShot::move(Context& context) {
  // moving
  x += 8.f;

  // frame out
  if(x > SCREEN_WIDTH) {
    inactivate();
  }
}

void AutoShot::draw(Context& context) {
  context.core.drawBitmap(x + 2, y + 3, bitmapAutoShot, 2);
  //context.core.getArduboy().drawRect(x, y, W, H, 1);
}


// === BigEnemy ===

void BigEnemy::initialize(const float y) {
  activate(FIELD_WIDTH, y);
  grazed = false;
  timer  = 0;
}

void BigEnemy::move(Context& context) {
  // moving
  static const float NORMAL_SPD = -0.2f;
  if(grazed && x > SCREEN_WIDTH - 5) {
    x += NORMAL_SPD * 16.f; // submarine found
  }
  else {
    x += NORMAL_SPD;
  }

  // frame out
  if(x + bitmapCruEnemy0[0] < 0.f) {
    inactivate();
  }

  // setting sonar reaction
  context.addEcho(x, y, y+H);

  // firing bullet
  if(timer == 0 && x < SCREEN_WIDTH - W / 2.f) {
    const float by = y +  bitmapCruEnemy0[1] / 2;
    context.fireBullet(x, by, context.getFutureSubmarineAngle(x, by, BULLET_TYPE0_SPD), 0);
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
  context.spawnParticle(x, y, 0);
  context.core.playScore(bing);
#ifndef LOW_MEMORY
  if(x < SCREEN_WIDTH + 20) {
    context.removeAllBullets();
    // ToDo: quake
  }
#endif
  inactivate();
}


// === SmallEnemy ===

void SmallEnemy::initialize(const float y, const byte type) {
  activate(FIELD_WIDTH, y);
  this->type = type;
  timer = 0;
}

void SmallEnemy::move(Context& context) {
  const byte period = (type / 2 == 0) ? 96 : 128;

  switch(type / 2) {
    // zigzag
    case 0: {
      // moving
      static const float VY = 0.6f;
      x += -VY * 2.4f;
      y += (timer / (ZIG_PERIOD / 4) % 2 == 0) ? VY : -VY;
      
      // firing bullet
      if(timer == 88 && type % 2 == 0 && x < SCREEN_WIDTH - W / 2.f) {
        context.fireBullet(x, y, context.getSubmarineAngle(x, y), 1);
      }
    } break;

    // triangle
    default: {
      // moving
      x += -0.1f - 5.f / (timer % (period / 2) + 1);
      
      // firing bullet
      if(timer == 64 && type % 2 == 0 && x < SCREEN_WIDTH - W / 2.f) {
        context.fireBullet(x, y, context.getSubmarineAngle(x, y), 1);
      }
    } break;
  }

  // frame out
  if(x + 12 < 0.f) {
    inactivate();
  }

  // setting sonar reaction
  context.addEcho(x, y, y+H);

  // updating timer
  timer = (timer + 1) % (type == 0 ? ZIG_PERIOD : TRI_PERIOD);
}

void SmallEnemy::draw(Context& context) {
  if(x - 4 > SCREEN_WIDTH) { return; }
  switch(type / 2) {
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
  context.spawnParticle(x - 5, y - 4, 0);
  inactivate();
}


// === Bullet ===

void Bullet::initialize(const float sx, const float sy, const float radian, const byte type) {
  activate(sx, sy);
  angle = radian;
  this->type = type;
}

void Bullet::move(Context& context) {
  const float speed = (type == 0 ? BULLET_TYPE0_SPD : BULLET_TYPE1_SPD);
  x += speed * cos(angle);
  y += speed * sin(angle);

  // frame out
  static const byte MARGIN = 4;
  if(
    x < -MARGIN || x > SCREEN_WIDTH  + MARGIN ||
    y < -MARGIN || y > SCREEN_HEIGHT + MARGIN
  ) {
    inactivate();  
  }
}

void Bullet::draw(Context& context) {
  // ToDo: async animation (if there are enough memories)
  const byte frame = context.frameCount() / 3 % 2;
  
  if(type == 0) {
    const byte* bitmaps[] = {bitmapMbullet0, bitmapMbullet1};
    context.core.drawBitmap(x - bitmapMbullet0[0]/2, y - bitmapMbullet0[1]/2, bitmaps[frame], 2);
  }
  else {
    const byte* bitmaps[] = {bitmapSbullet0, bitmapSbullet1};
    context.core.drawBitmap(x - bitmapSbullet0[0]/2, y - bitmapSbullet0[1]/2, bitmaps[frame], 2);
  }
  //arduboy.drawPixel(x, y, 0);
}

void Bullet::onHit(Context& context) {
  context.core.drawCircle(x, y, 6, 1);
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

