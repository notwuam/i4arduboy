#pragma once

#include "constants.h"

#define RANKING_NAME_LEN   (3)
#define RANKING_ENTRY_MAX  (5)
#define EEPROM_ENTRY_BYTES (RANKING_NAME_LEN + sizeof(unsigned int))

struct Ranking {
  // as scene
  void initialize() {
#ifdef USE_RANKING_EEPROM
    // file index bases on sample sketch "ArduBreakout"
    // > Each block of EEPROM has 10 high scores, and each high score entry
    // > is 5 bytes long:  3 bytes for initials and two bytes for score.
    int baseAddress = EEPROM_FILE_INDEX * 5 * 10;
    for(byte i = 0; i < RANKING_ENTRY_MAX; ++i) {
      // ToDo: load from eeprom
    }
#else
    for(byte i = 0; i < RANKING_ENTRY_MAX; ++i) {
      scores[i] = 0;
    }
    for(byte i = 0; i < RANKING_ENTRY_MAX * RANKING_NAME_LEN; ++i) {
      names[i]  = ' ';
    }
#endif
  }
  
  bool loop(GameCore& core) {
    if(core.pushed(BTN_A) || core.pushed(BTN_B)) {
      return true;
    }
    char text[16];
    for(byte i = 0; i < RANKING_ENTRY_MAX; ++i) {
      sprintf(text, "%1d %c%c%c %05d", i+1, names[i*3], names[i*3+1], names[i*3+2], scores[i]);
      core.setCursor(0, i*8);
      core.print(text);
    }
    return false;
  }
  
  // as scoring manager
  // returns 0-4. +1 when display
  byte getRank(const unsigned int score) const {
    byte rank;
    for(rank = 0; rank < RANKING_ENTRY_MAX; ++rank) {
      if(score > scores[rank]) { break; }
    }
    return rank;
  }
  void enterScore(const unsigned int score, const char* name) {
    const byte rank = getRank(score);
    if(rank >= RANKING_ENTRY_MAX) { return; }
    // slide entry
    for(char i = RANKING_ENTRY_MAX - 2; i >= rank; --i) {
      scores[i+1]      = scores[i];
      names[(i+1)*3  ] = names[i*3  ];
      names[(i+1)*3+1] = names[i*3+1];
      names[(i+1)*3+2] = names[i*3+2];
    }
    // enter
    scores[rank]    = score;
    names[rank*3  ] = name[0];
    names[rank*3+1] = name[1];
    names[rank*3+2] = name[2];
#ifdef USE_RANKING_EEPROM
    // ToDo: write eeprom
#endif
  }

  private:
  unsigned int scores[RANKING_ENTRY_MAX];
  char names[RANKING_ENTRY_MAX * RANKING_NAME_LEN];
};

