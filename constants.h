#pragma once

#define DEBUG

// usually commented out
//#define EXHIBITION_MODE

// define to omit some specifications in order to reduce flash memory
//#define LOW_FLASH_MEMORY

// define if use EEPROM for saving ranking data
//#define USE_RANKING_EEPROM

// file index bases on sample sketch "ArduBreakout"
// > Each block of EEPROM has 10 high scores, and each high score entry
// > is 5 bytes long:  3 bytes for initials and two bytes for score.
// so base address is EEPROM_FILE_INDEX * 5(bytes) * 10(hi-scores)
#define EEPROM_FILE_INDEX  (3)

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
#define EXTEND_SCORE     (500)

#define SCENE_ENTRY_WAIT (30)

