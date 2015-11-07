#pragma once

bool Collision(
  const float x1, const float y1, const byte w1, const byte h1, 
  const float x2, const float y2, const byte w2, const byte h2
);

float Clamp(const float value, const float min, const float max);

void DrawWave(GameCore& core, const int beginX, const unsigned long frameCount);
void DrawBottom(GameCore& core, const int beginX);

