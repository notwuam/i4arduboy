#include "gamecore.h"
#include "systembitmaps.h"

bool Collision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
  if(x1 + w1 < x2 || x1 > x2 + w2) return false;
  if(y1 + h1 < y2 || y1 > y2 + h2) return false;
  return true;
}

void DrawWave(GameCore& core, const int beginX, const long frameCount) {
  const int w = bitmapWave0[0];
  const int x = beginX % (w * 4);
  for(int i = 0; i < SCREEN_WIDTH / w + 4; ++i) {
    const int xx = x + w * i;
    if(xx > SCREEN_WIDTH) { break; }
    if(xx + w < 0) { continue; }
    switch((frameCount / 20 + i) % 4) {
      case 0:  core.drawBitmap(xx, 0, bitmapWave0, 1); break;
      case 1:  core.drawBitmap(xx, 0, bitmapWave1, 1); break;
      case 2:  core.drawBitmap(xx, 0, bitmapWave2, 1); break;
      default: core.drawBitmap(xx, 0, bitmapWave3, 1); break;
    }
  }
}

void DrawBottom(GameCore& core, const int beginX) {
  const int w = bitmapBottom[0];
  const int h = bitmapBottom[1];
  const int x = beginX % w;
  core.drawBitmap(x, SCREEN_HEIGHT - h, bitmapBottom, 1);
  core.drawBitmap(x + w, SCREEN_HEIGHT - h, bitmapBottom, 1);
  if(x + w * 2 < SCREEN_WIDTH) {
    core.drawBitmap(x + w * 2, SCREEN_HEIGHT - h, bitmapBottom, 1);
  }
}

