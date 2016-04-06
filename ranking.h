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
      const int entryAddress = baseAddress + i * EEPROM_ENTRY_BYTES;
      scores[i]  = EEPROM.read(entryAddress) << 8;
      scores[i] |= EEPROM.read(entryAddress + 1);
      if(scores[i] == 0xffff) {
        scores[i] = 0;
        names[i*RANKING_NAME_LEN  ] = ' ';
        names[i*RANKING_NAME_LEN+1] = ' ';
        names[i*RANKING_NAME_LEN+2] = ' ';
      }
      else {
        names[i*RANKING_NAME_LEN  ] = (char)EEPROM.read(entryAddress + 2);
        names[i*RANKING_NAME_LEN+1] = (char)EEPROM.read(entryAddress + 3);
        names[i*RANKING_NAME_LEN+2] = (char)EEPROM.read(entryAddress + 4);
      }
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
  
  void onEntry() {
    waitTimer = SCENE_ENTRY_WAIT; // for disabling input for a while
  }
  
  bool loop(GameCore& core) {
    // return title menu
    if((core.pushed(BTN_A) || core.pushed(BTN_B)) && waitTimer <= 0) {
      return true;
    }

    // draw
    // background
    DrawWave(core, 0, core.frameCount());
    DrawBottom(core, 0);
    // entries
    char text[16];
    for(byte i = 0; i < RANKING_ENTRY_MAX; ++i) {
      sprintf(text, "%1d  %c%c%c  %05d", i+1, names[i*RANKING_NAME_LEN], names[i*RANKING_NAME_LEN + 1], names[i*RANKING_NAME_LEN + 2], scores[i]);
      core.setCursor(26, i * 10 + 9);
      core.print(text);
    }
    
    if(waitTimer > 0) { --waitTimer; }
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
      scores[(byte)i+1] = scores[(byte)i];
      const byte to   = (i+1)*RANKING_NAME_LEN;
      const byte from = i*RANKING_NAME_LEN;
      names[to  ] = names[from  ];
      names[to+1] = names[from+1];
      names[to+2] = names[from+2];
    }
    // enter
    scores[rank] = score;
    names[rank*RANKING_NAME_LEN  ] = name[0];
    names[rank*RANKING_NAME_LEN+1] = name[1];
    names[rank*RANKING_NAME_LEN+2] = name[2];
    
#ifdef USE_RANKING_EEPROM
    int baseAddress = EEPROM_FILE_INDEX * 5 * 10;
    for(byte i = 0; i < RANKING_ENTRY_MAX; ++i) {
      const int entryAddress = baseAddress + i * EEPROM_ENTRY_BYTES;
      EEPROM.write(entryAddress    , (scores[i] >> 8) & 0xff);
      EEPROM.write(entryAddress + 1, scores[i] & 0xff);
      EEPROM.write(entryAddress + 2, (byte)names[i*RANKING_NAME_LEN  ]);
      EEPROM.write(entryAddress + 3, (byte)names[i*RANKING_NAME_LEN+1]);
      EEPROM.write(entryAddress + 4, (byte)names[i*RANKING_NAME_LEN+2]);
    }
#endif
  }

  private:
  unsigned int scores[RANKING_ENTRY_MAX];
  char names[RANKING_ENTRY_MAX * RANKING_NAME_LEN];
  byte waitTimer;
};

