#include <SPI.h>
#include <EEPROM.h>
#include "Arduboy.h"

#include "gamecore.h"
#include "gamelevel.h"
#include "title.h"
#include "nameentry.h"
#include "ranking.h"

enum {
  SCENE_TITLE = 0,
  SCENE_GAME,
  SCENE_NAME_ENTRY,
  SCENE_RANKING,
};

GameCore core;
Title     title;
GameLevel gameLevel(core);
NameEntry nameEntry;
Ranking   ranking;

byte scene = SCENE_TITLE;

void setup() {
  //Serial.begin(9600);
  core.setup();
  ranking.initialize();
}

void loop() {
  if(!(core.nextFrame())) {
    return;
  }
  core.clearDisplay();
  core.updateInput();
#ifndef LOW_FLASH_MEMORY
  core.updateQuake();
#endif

  switch(scene) {
    case SCENE_TITLE: {
      switch(title.loop(core)) {
        case TITLE_START_GAME: {
          gameLevel.onEntry();
          scene = SCENE_GAME;
        } break;
          
        case TITLE_DISP_RANKING: {
          ranking.onEntry();
          scene = SCENE_RANKING;
        } break;
        
        default: break;
      }
    } break;
    
    case SCENE_GAME: {
      if(gameLevel.loop()) {
        // check high score
        const byte rank = ranking.getRank(gameLevel.getScore());
        if(rank < RANKING_ENTRY_MAX && gameLevel.getScore() > 0) {
          nameEntry.onEntry(rank, gameLevel.getScore());
          scene = SCENE_NAME_ENTRY;
        }
        else {
          ranking.onEntry();
          scene = SCENE_TITLE;  // didnt rank in
        }
      }
      // A+B+Left to terminate
      if(core.pressed(BTN_A) && core.pressed(BTN_B) && core.pressed(BTN_L)) {
        title.onEntry();
        scene = SCENE_TITLE;
      }
    } break;

    case SCENE_NAME_ENTRY: {
      if(nameEntry.loop(core)) {
        ranking.enterScore(nameEntry.getScore(), nameEntry.getName());
        title.onEntry();
        scene = SCENE_TITLE;
      }
    } break;

    default: {
      if(ranking.loop(core)) {
        title.onEntry();
        scene = SCENE_TITLE;
      }
    } break;
  }
  
  /*
  char text[16];
  sprintf(text, "%d,%d,%d", getType(), canFire(), getPlatoon());
  context.core.setCursor(10, 10);
  context.core.print(text);
  */
  
  core.display();
}

