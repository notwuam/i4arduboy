#pragma once

bool Collision(
  const int x1, const char y1, const byte w1, const byte h1, 
  const int x2, const char y2, const byte w2, const byte h2
);

int Clamp(const int value, const int min, const int max);

void DrawWave(GameCore& core, const int beginX, const unsigned long frameCount);
void DrawBottom(GameCore& core, const int beginX);

