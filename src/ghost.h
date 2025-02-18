#ifndef GHOST_H_
#define GHOST_H_
#include "game.h"

typedef struct Ghost Ghost;
struct Ghost {
  Position position;
  Direction direction;
  bool immortal;
  DirectionName directionName;
  char icon[4];
  char prevSymbol;
  int id;
};

DirectionName getNextGhostDirection(Game *game, Ghost *ghost);
void updateGhost(Game *game, Ghost *ghost);
void renderGhosts(Game *game);

#endif
