#include "echo.h"
#include "context.h"

void Echo::reset(Context& context, float subX) {
  if(context.frameCount() % ECHO_CYCLE == 0) {
    for(int i = 0; i < ECHO_VERT_RESO; ++i) {
      intensities[i] = 0;
    }
    acceptFlag = true;
  }
  else {
    acceptFlag = false;
  }
  submarineX = subX;
}

void Echo::add(float left, float top, float bottom) {
  if(!acceptFlag) { return; }
  
  const float dist = left - submarineX;
  if(dist < 0.f) { return; }
  
  const int b = round(top) / ECHO_GRID_SIZE;
  const int e = round(bottom) / ECHO_GRID_SIZE;
  unsigned char newInte = distToIntensity(dist);
  
  for(int i = b; i <= e; ++i) {
    if(newInte > intensities[i]) {
      intensities[i] = newInte;
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
  for(int i = 0; i < ECHO_VERT_RESO; ++i) {
    for(int j = 0; j < intensities[i]; ++j) {
      context.core.fillRect(SCREEN_WIDTH - (j - 1) * ECHO_GRID_SIZE, i * ECHO_GRID_SIZE, ECHO_GRID_SIZE - 1, ECHO_GRID_SIZE - 1, 1);
    }
  }
}

