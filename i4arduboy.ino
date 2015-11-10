#include <SPI.h>
#include <EEPROM.h>
#include "Arduboy.h"

#include "gamecore.h"
#include "context.h"
#include "title.h"
#include "nameregist.h"

enum {
  SCENE_TITLE = 0,
  SCENE_GAME,
  SCENE_NAMEREGIST,
};

GameCore   core;
Title      title;
Context    context(core);
NameRegist nameRegist;

byte scene = SCENE_NAMEREGIST;

void setup() {
  //Serial.begin(9600);
  core.setup();
  context.initialize();
  
  nameRegist.initialize(1, 3000); // test
}

void loop() {
  if(!(core.nextFrame())) {
    return;
  }
  core.clearDisplay();
  core.updateInput();

  switch(scene) {
    case SCENE_TITLE: {
      switch(title.loop(core)) {
        case TITLE_START_GAME:
          context.initialize();
          scene = SCENE_GAME;
          break;

        default: break;
      }
    } break;
    
    case SCENE_GAME: {
      if(context.loop()) {
        scene = SCENE_NAMEREGIST;
        nameRegist.initialize(1, 3000); // test data
      }
    } break;

    default: {
      if(nameRegist.loop(core)) {
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

