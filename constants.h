#pragma once

// for exist flag
#define EXIST_THRESHOLD  (-64.f)

// included invisible area
#define FIELD_WIDTH      (SCREEN_WIDTH * 3)

// each character numbers
#define AUTO_SHOT_MAX    (2)
#define BIG_ENEMY_MAX    (8)
#define SMALL_ENEMY_MAX  (12)
#define BULLET_MAX       (8)
#define PARTICLE_MAX     (8)

// submarine settings
#define START_LIVES      (2)
#define ARMER_FRAMES     (90)

#define BULLET_TYPE0_SPD (3.f)
#define BULLET_TYPE1_SPD (1.f)

enum {
  SENEMY_ZIG_FIRE = 0,
  SENEMY_ZIG_NOFIRE,
  SENEMY_TRI_FIRE,
  SENEMY_TRI_NOFIRE,
};

