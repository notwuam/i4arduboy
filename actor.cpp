#include "actor.h"
#include "context.h"
#include "echo.h"
#include "util.h"
#include "actorbitmaps.h"
#include "scores.h"

// === Submarine ===

void Submarine::initialize() {
  activate(W, SCREEN_HEIGHT / 2.f);
  prevFire   = true;  // to prevent from launching a torpedo
  extraLives = START_LIVES;
  armer      = ARMER_FRAMES;
}

void Submarine::move(Context& context) {
  static const float SPD   = 0.5f;
  static const float SQRT2 = 0.7071067811f;
  
  // control
  if(context.core.pressed(BTN_U) && context.core.pressed(BTN_L)) {
    x -= SPD * SQRT2;
    y -= SPD * SQRT2;
  }
  else if(context.core.pressed(BTN_U) && context.core.pressed(BTN_R)) {
    x += SPD * SQRT2;
    y -= SPD * SQRT2;
  }
  else if(context.core.pressed(BTN_D) && context.core.pressed(BTN_L)) {
    x -= SPD * SQRT2;
    y += SPD * SQRT2;
  }
  else if(context.core.pressed(BTN_D) && context.core.pressed(BTN_R)) {
    x += SPD * SQRT2;
    y += SPD * SQRT2;
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
  static const int MARGIN = 6;
  x = Clamp(x, MARGIN - W / 2.f, SCREEN_WIDTH  - MARGIN - W / 2.f);
  y = Clamp(y, MARGIN - H / 2.f, SCREEN_HEIGHT - MARGIN - H / 2.f);

  // launching torpedo
  if(extraLives >= 0 && (context.core.pressed(BTN_A) || context.core.pressed(BTN_B))) {
    if(!prevFire) {
      context.tryLaunchTorpedo(x + 10, y + 1);
    }
    prevFire = true;
  }
  else {
    prevFire = false;
  }

  // updating armer timer
  if(armer > 0) { --armer; }
  if(extraLives < 0 && armer <= 0) {
    --extraLives; // to exit from this game
  }
}

void Submarine::draw(Context& context) {
  if(armer == 0 || (extraLives >= 0 && context.frameCount() / 3 % 2 == 0)) {
    context.core.drawBitmap(x - 1, y - 2, bitmapSubmarine, 2);
    if(context.frameCount() % ECHO_CYCLE > 30) {  // sustain: 30 frames
      context.core.drawPixel(round(x + 6), round(y - 1), 1);
    }
  }
  //context.getArduboy().drawRect(x, y, W, H, 0);
}

void Submarine::onHit(Context& context) {
  if(armer <= 0) {
    if(extraLives >= 0) {
      context.spawnParticle(x - 2, y - 2, 0);
    }
    --extraLives;
    armer = ARMER_FRAMES;
  }
}


// === Torpedo ===

void Torpedo::initialize() {
  inactivate();
}

bool Torpedo::tryLaunch(const float sx, const float sy) {
  if(!exist()) {
    activate(sx, sy);
    vx = 0.f;
    return true;
  }
  return false;
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
    const int w = bitmapShockwave0[0];
    const int h = bitmapShockwave0[1];
    context.core.drawBitmap(x -  8, y - h/2, bitmapShockwave0, 2);
    context.core.drawBitmap(x - 16, y - h/2, bitmapShockwave1, 2);
    context.core.drawBitmap(x - 24, y - h/2, bitmapShockwave2, 2);
  }
  //context.getArduboy.drawFastHLine(x, y, W, 0);
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
  if(timer == 0) {
    const float by = y +  bitmapCruEnemy0[1]/2;
    context.fireBullet(x, by, context.getFutureSubmarineAngle(x, by, BULLET_TYPE0_SPD), 0);
  }

  // updating timer
  static const unsigned char PERIOD = 150;
  timer = (timer + 1) % PERIOD;
}

void BigEnemy::draw(Context& context) {
  if(x - 3 > SCREEN_WIDTH) { return; }
  context.core.drawBitmap(x - 3, y - 2, bitmapCruEnemy0, 2);
}

void BigEnemy::onHit(Context& context) {
  context.spawnParticle(x, y, 0);
  context.core.playScore(bing);
  inactivate();
}


// === Bullet ===

void Bullet::initialize(const float sx, const float sy, const float radian, const unsigned char type) {
  activate(sx, sy);
  angle = radian;
  this->type = type;
}

void Bullet::move(Context& context) {
  const float speed = (type == 0 ? BULLET_TYPE0_SPD : BULLET_TYPE1_SPD);
  x += speed * cos(angle);
  y += speed * sin(angle);

  // frame out
  static const int MARGIN = 4;
  if(
    x < -MARGIN || x > SCREEN_WIDTH  + MARGIN ||
    y < -MARGIN || y > SCREEN_HEIGHT + MARGIN
  ) {
    inactivate();  
  }
}

void Bullet::draw(Context& context) {
  // ToDo: async animation (if there are enough memories)
  const int frame = context.frameCount() / 3 % 2;
  
  if(type == 0) {
    const unsigned char* bitmaps[] = {bitmapMbullet0, bitmapMbullet1};
    context.core.drawBitmap(x - bitmapMbullet0[0]/2, y - bitmapMbullet0[1]/2, bitmaps[frame], 2);
  }
  else {
    const unsigned char* bitmaps[] = {bitmapSbullet0, bitmapSbullet1};
    context.core.drawBitmap(x - bitmapSbullet0[0]/2, y - bitmapSbullet0[1]/2, bitmaps[frame], 2);
  }
  //arduboy.drawPixel(x, y, 0);
}

void Bullet::onHit(Context& context) {
  inactivate();
}


// === Particle ===

void Particle::initialize(const float x, const float y, const unsigned char type) {
  if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) { return; }
  activate(x, y);
  this->type;
  switch(type) {
    default: limit = 12; break;
  }
}

void Particle::move(Context& context) {
  --limit;
  if(limit <= 0) {
    inactivate();
  }
}

void Particle::draw(Context& context) {
  switch(type) {
    default: {
      if(limit > 8) {
        context.core.drawBitmap(x, y, bitmapExplosion0, 2);
      }
      else if(limit > 4) {
        context.core.drawBitmap(x, y, bitmapExplosion1, 2);
      }
      else {
        context.core.drawBitmap(x, y, bitmapExplosion2, 2);
      }
    }
  }
}

