#include "modules.h"
#include "context.h"

// === Echo ===

void Echo::reset(Context& context, const byte subX) {
  if(context.frameCount() % ECHO_CYCLE == 0) {
    for(byte i = 0; i < ECHO_VERT_RESO; ++i) {
      intensities[i] = 0;
    }
    acceptFlag = true;
  }
  else {
    acceptFlag = false;
  }
  submarineX = subX;
}

void Echo::add(const int left, const char top, const char bottom) {
  if(!acceptFlag) { return; }
  
  const int dist = left - submarineX;
  if(dist < 0) { return; }
  
  const byte b = top    / ECHO_GRID_SIZE;
  const byte e = bottom / ECHO_GRID_SIZE;
  byte newInte = (byte)((FIELD_WIDTH - dist) / 20);
  
  for(byte i = b; i <= e; ++i) {
    if(newInte > intensities[i]) {
      intensities[i] = newInte; // update
    }
  }
}

void Echo::draw(Context& context) {
  if(
    context.frameCount() / 3 % 2 == 0 ||  // tearing
    context.frameCount() % ECHO_CYCLE < ECHO_CYCLE / 2  // sustain: 30 frames
  ) {
    return;
  }
  for(byte i = 0; i < ECHO_VERT_RESO; ++i) {
    for(byte j = 0; j < intensities[i]; ++j) {
      context.core.fillRect(SCREEN_WIDTH - (j - 1) * ECHO_GRID_SIZE, i * ECHO_GRID_SIZE, ECHO_GRID_SIZE - 1, ECHO_GRID_SIZE - 1, 1);
    }
  }
}


// === Platoons ===

void Platoons::initialize() {
  for(byte i = 0; i < PLATOON_MAX; ++i) {
    status[i] = 0;
  }
}

void Platoons::set(const char y, const byte type) {
  for(byte i = 0; i < PLATOON_MAX; ++i) {
    if(!inUse(i)) {
      spawnYs[i] = y;
      types[i]   = type;
      timers[i]  = 0;
      status[i]  = PLATOON_USING_MASK;
      return;
    }
  }
}

void Platoons::spawn(Context& context) {
  static const byte INTERVAL = 16;
  for(byte i = 0; i < PLATOON_MAX; ++i) {
    if(!inUse(i)) { continue; }
    if(timers[i] % INTERVAL == 0 && timers[i] < PLATOON_CONSISTS * INTERVAL) {
      if(context.spawnSmallEnemy(spawnYs[i], (i << 4) | types[i])) {
        status[i] = 0;
      }
    }
    ++timers[i];
  }
}

bool Platoons::checkBonus(const byte idx, bool killed) {
  if(inUse(idx)) { return false; }
  
  // count
  ++status[idx];
  if(killed) { status[idx] += (1 << 3); }

  // check
  if((status[idx] & PLATOON_COUNT_MASK) >= PLATOON_CONSISTS) {
    if(((status[idx] >> 3) & PLATOON_COUNT_MASK) >= PLATOON_CONSISTS) {
      status[idx] = 0;
      return true;
    }
    status[idx] = 0;
  }
  return false;
}

