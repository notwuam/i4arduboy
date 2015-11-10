#pragma once

// define to omit some specifications in order to reduce flash memory
//#define LOW_MEMORY

// define if use EEPROM for saving ranking data
#define USE_RANKING_EEPROM
// file index bases on sample sketch "ArduBreakout"
// > Each block of EEPROM has 10 high scores, and each high score entry
// > is 5 bytes long:  3 bytes for initials and two bytes for score.
// so base address is EEPROM_FILE_INDEX * 5(bytes) * 10(hi-scores)
#define EEPROM_FILE_INDEX  (3)
#define RANKING_NAME_LEN   (3)
#define EEPROM_ENTRY_BYTES (RANKING_NAME_LEN + sizeof(unsigned int))

// for exist flag
#define EXIST_THRESHOLD       (-64)
#define FIXED_EXIST_THRESHOLD (-(64 << 8))

// included invisible area
#define FIELD_WIDTH      (SCREEN_WIDTH * 3)

// each character numbers
#define AUTO_SHOT_MAX    (2)
#define BIG_ENEMY_MAX    (8)
#define SMALL_ENEMY_MAX  (20)
#define BULLET_MAX       (8)
#define PARTICLE_MAX     (8)

// submarine settings
#define START_LIVES      (2)
#define ARMER_FRAMES     (90)

// speed of bullets
#define BULLET_TYPE0_SPD (3)
#define BULLET_TYPE1_SPD (1)

enum {
  SENEMY_ZIG_FIRE = 0,
  SENEMY_ZIG_NOFIRE,
  SENEMY_TRI_FIRE,
  SENEMY_TRI_NOFIRE,
};

