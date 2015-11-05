#pragma once

bool Collision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);

void DrawWave(GameCore& core, const int beginX, const long frameCount);
void DrawBottom(GameCore& core, const int beginX);

