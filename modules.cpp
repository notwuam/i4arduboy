#include "modules.h"
#include "gamelevel.h"

// === Echo ===

void Echo::reset(GameLevel& context, const byte subX) {
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
  byte newInte = (byte)((FIELD_WIDTH - dist) / ECHO_HORI_RESO) + 3; // +3 for display soon
  
  for(byte i = b; i <= e; ++i) {
    if(newInte > intensities[i]) {
      intensities[i] = newInte; // update
    }
  }
}

void Echo::draw(GameLevel& context) const {
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

void Platoons::spawn(GameLevel& context) {
  const byte INTERVALS[] = {10, 16, 16};
  for(byte i = 0; i < PLATOON_MAX; ++i) {
    if(!inUse(i)) { continue; }
    if(
      timers[i] % INTERVALS[types[i]] == 0 &&
      timers[i] < PLATOON_CONSISTS * INTERVALS[types[i]]  // limitation PLATOON_CONSISTS
    ) {
      // type
      const byte isOdd = timers[i] / INTERVALS[types[i]] % 2 == 1; // 0123 -> 1234
      byte spawnType;
      if(types[i] == PLATOON_ZIG_FILE) {
        spawnType = isOdd ? SENEMY_ZIG_FIRE : SENEMY_ZIG_NOFIRE;
      }
      else {
        spawnType = isOdd ? SENEMY_TRI_FIRE : SENEMY_TRI_NOFIRE;
      }
      // y
      char y = spawnYs[i];
      if(types[i] == PLATOON_TRI_SHOAL) {
        y += random(20) - 10;
      }
      // spawn
      if(context.spawnSmallEnemy(y, (i << 4) | spawnType)) {
        status[i] = 0;  // reset platoon
      }
    }
    if(timers[i] < 0xff) {
      ++timers[i];
    }
  }
}

bool Platoons::checkBonus(const byte idx, bool killed) {
  if(!inUse(idx)) { return false; }
  
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


// === Generator ===

void Generator::initialize() {
  difficulty = 0;
  zone       = 0;
  waveCount  = WAVES_IN_ZONE - 1;
  waveIndex  = 0;
  progCount  = 0;
  delayTimer = 0;//255;
  dispTimer  = 0;
}

void Generator::spawn(GameLevel& context) {
#ifdef DEBUG
  if(context.core.pressed(BTN_B)) {
    if(context.core.pushed(BTN_U)) { difficulty += 10; }
    if(context.core.pushed(BTN_D)) { difficulty -= 10; }
  }
#endif

  if(dispTimer  > 0) { --dispTimer; }
  if(delayTimer > 0) { --delayTimer; return; }  // if delaying, skip generating step
  
  const byte* waves[] = { // generating scripts
    waveTest, waveBeginner0, waveCamouflage0, waveCamouflage1, waveBigWall
  };
  static const byte WAVE_PATTERN_MAX = 5;
  byte inst;  // current instruction (or operand)

  inst = pgm_read_byte(waves[waveIndex] + progCount); // fetch
  while(inst != INST_END_WAVE && delayTimer <= 0) {
    // spawn
    if((inst & INST_SPAWN_MASK) != 0) {
      const byte type = inst & INST_TYPE_MASK;
      
      // get next operand
      ++progCount;
      inst = pgm_read_byte(waves[waveIndex] + progCount);
      const byte rand = inst & INST_RAND_MASK;
      if(rand == INST_RAND_ERROR) { break; }  // invalid operand
      byte y = inst & INST_Y_MASK;
      if     (rand == INST_RAND_WIDE  ) { y =  random(4, 57);  }
      else if(rand == INST_RAND_NARROW) { y += random(10) - 5; }
      
      switch(type) {
        case SPAWN_BIG:        context.spawnBigEnemy  (y);                      break;
        case SPAWN_ZIG_SINGLE: context.spawnSmallEnemy(y-5, SENEMY_ZIG_FIRE  ); break;
        case SPAWN_ZIG_FILE:   context.platoons.set   (y-5, PLATOON_ZIG_FILE ); break;
        case SPAWN_TRI_SINGLE: context.spawnSmallEnemy(y  , SENEMY_TRI_FIRE  ); break;
        case SPAWN_TRI_LINE:   context.platoons.set   (y  , PLATOON_TRI_LINE ); break;
        case SPAWN_TRI_SHOAL:  context.platoons.set   (y  , PLATOON_TRI_SHOAL); break;
        default: break;
      }
    }
    // delay
    else {
      delayTimer = inst;
    }
    
    // get next instruction
    ++progCount;
    inst = pgm_read_byte(waves[waveIndex] + progCount);
  }

  // next wave
  static const byte ZONE_DISP_FRAMES = 90;
  static const byte DIFFICULTY_INCR  = 20;
  static const byte DIFFICULTY_DECR  = 15;
  if(inst == INST_END_WAVE && delayTimer <= 0) {
    ++waveCount;
    progCount = 0;
    
    // ToDo: difficulty limit
    waveIndex = random(WAVE_PATTERN_MAX - 1) + 1; // except zero
    
    // difficulty up
    if(difficulty < DIFFICULTY_CAP + DIFFICULTY_DECR) {
      difficulty += DIFFICULTY_INCR / WAVES_IN_ZONE;
    }
    
    // next zone
    if(waveCount >= WAVES_IN_ZONE) {
      waveCount = 0;
      dispTimer = ZONE_DISP_FRAMES;  // disp zone and score
      ++zone;
      // difficulty down
      if(difficulty < DIFFICULTY_CAP + DIFFICULTY_DECR && zone > 1) {
        difficulty -= DIFFICULTY_DECR;
      }
    }
  }
}

void Generator::draw(GameLevel& context) const {
  if(dispTimer > 0 && context.frameCount() / 5 % 2 == 0) {
    char text[12];
    sprintf(text, "Score %05d", context.getScore());
    context.core.setCursor(SCREEN_WIDTH / 2 - 6 * 11 / 2, 0);
    context.core.print(text);
    sprintf(text, "ZONE%3d", zone);
    context.core.setCursor(SCREEN_WIDTH / 2 - 6 * 7 / 2, 10);
    context.core.print(text);
  }
#ifdef DEBUG
  char text[12];
  sprintf(text, "%d", waveCount);
  context.core.setCursor(0, 56);
  context.core.print(text);
#endif
}

