#include "gamecore.h"
#include "systembitmaps.h"

bool Collision(
  const float x1, const float y1, const byte w1, const byte h1, 
  const float x2, const float y2, const byte w2, const byte h2
) {
  if(x1 + w1 < x2 || x1 > x2 + w2) { return false; }
  if(y1 + h1 < y2 || y1 > y2 + h2) { return false; }
  return true;
}

float Clamp(const float value, const float min, const float max) {
  if(value < min) { return min; }
  if(value > max) { return max; }
  return value;
}

void DrawWave(GameCore& core, const int beginX, const unsigned long frameCount) {
  const byte* bitmaps[] = {bitmapWave0, bitmapWave1, bitmapWave2, bitmapWave3};
  const byte w = bitmapWave0[0];
  const byte x = beginX % (w * 4);
  for(byte i = 0; i < SCREEN_WIDTH / w + 4; ++i) {
    const byte xx = x + w * i;
    if(xx > SCREEN_WIDTH) { break; }
    if(xx + w < 0) { continue; }
    core.drawBitmap(xx, 0, bitmaps[(frameCount / 20 + i) % 4], 1);
  }
}

void DrawBottom(GameCore& core, const int beginX) {
  const byte w = bitmapBottom[0];
  const byte h = bitmapBottom[1];
  const byte x = beginX % w;
  core.drawBitmap(x, SCREEN_HEIGHT - h, bitmapBottom, 1);
  core.drawBitmap(x + w, SCREEN_HEIGHT - h, bitmapBottom, 1);
  if(x + w * 2 < SCREEN_WIDTH) {
    core.drawBitmap(x + w * 2, SCREEN_HEIGHT - h, bitmapBottom, 1);
  }
}

