#pragma once

bool Collision(
  const float x1, const float y1, const int w1, const int h1, 
  const float x2, const float y2, const int w2, const int h2
);

float Clamp(const float value, const float min, const float max);

void DrawWave(GameCore& core, const int beginX, const long frameCount);
void DrawBottom(GameCore& core, const int beginX);

