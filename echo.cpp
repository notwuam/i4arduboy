#include "echo.h"
#include "context.h"

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

void Echo::add(const float left, const float top, const float bottom) {
  if(!acceptFlag) { return; }
  
  const float dist = left - submarineX;
  if(dist < 0.f) { return; }
  
  const byte b = round(top) / ECHO_GRID_SIZE;
  const byte e = round(bottom) / ECHO_GRID_SIZE;
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

