#ifndef PLAYER_H_
#define PLAYER_H_
#include "game.h"

typedef struct Player Player;
struct Player {
  Position position;
  Direction direction;
  int lives;
  char icon[4];
  DirectionName directionName;
};

void updatePlayer(Game *game);
void renderPlayer(Game *game);

#endif
