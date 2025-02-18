#include "ghost.h"

DirectionName getNextGhostDirection(Game *game, Ghost *ghost) {
  // Массив для хранения шансов движения в разных направлениях
  int directionChance[4];
  directionChance[LEFT] = 0;
  directionChance[UP] = 0;
  directionChance[RIGHT] = 0;
  directionChance[DOWN] = 0;

  // Текущее направление движения
  DirectionName directionName = ghost->directionName;

  // Получаем символы, отображающие следующее положение в каждом направлении
  char leftSymbol = getNextSymbol(game, &ghost->position, &DIRECTION_LEFT);
  char upSymbol = getNextSymbol(game, &ghost->position, &DIRECTION_UP);
  char rightSymbol = getNextSymbol(game, &ghost->position, &DIRECTION_RIGHT);
  char downSymbol = getNextSymbol(game, &ghost->position, &DIRECTION_DOWN);

  // Если на текущем направлении движения нет препятствий, то шанс
  // продолжить движения в этом направлении более высокий (40%)
  if (leftSymbol != '#') {
    if (directionName == LEFT && directionChance[LEFT] == 0) {
      directionChance[LEFT] = 40;
      directionChance[RIGHT] = -1;
    } else {
      directionChance[LEFT] = RANDOM_BETWEEN(0, 100);
    }
  }

  // Если на текущем направлении движения нет препятствий, то шанс
  // продолжить движения в этом направлении более высокий (40%)
  if (upSymbol != '#') {
    if (directionChance[UP] != -1) {
      if (directionName == UP && directionChance[UP] == 0) {
        directionChance[UP] = 40;
        directionChance[DOWN] = -1;
      } else {
        directionChance[UP] = RANDOM_BETWEEN(0, 100);
      }
    }
  }

  // Если на текущем направлении движения нет препятствий, то шанс
  // продолжить движения в этом направлении более высокий (40%)
  if (rightSymbol != '#') {
    if (directionChance[RIGHT] != -1) {
      if (directionName == RIGHT && directionChance[RIGHT] == 0) {
        directionChance[RIGHT] = 40;
        directionChance[LEFT] = -1;
      } else {
        directionChance[RIGHT] = RANDOM_BETWEEN(0, 100);
      }
    }
  }

  // Если на текущем направлении движения нет препятствий, то шанс
  // продолжить движения в этом направлении более высокий (40%)
  if (downSymbol != '#') {
    if (directionChance[DOWN] != -1) {
      if (directionName == DOWN && directionChance[DOWN] == 0) {
        directionChance[DOWN] = 40;
        directionChance[UP] = -1;
      } else {
        directionChance[DOWN] = RANDOM_BETWEEN(0, 100);
      }
    }
  }

  // Находим направление с максимальным шансом
  int maxDirectionRate = 0;
  DirectionName resultDirection;

  for (int i = 0; i < 4; i++) {
    if (directionChance[i] >= maxDirectionRate) {
      maxDirectionRate = directionChance[i];
      resultDirection = (DirectionName)i;
    }
  }

  // Возвращаем новое направление
  return resultDirection;
}

// Работает по аналогии player.c::updatePlayer
void updateGhost(Game *game, Ghost *ghost) {

  // Вычисляем следующие координаты призрака на основе его текущего положения и
  // направления.
  const int nextPosX = ghost->position.x + ghost->direction.x;
  const int nextPosY = ghost->position.y + ghost->direction.y;

  // Следующий символ
  const char nextSymbol = map[nextPosY][nextPosX];

  if (nextSymbol == M_LEFT_PORTAL) {
    map[ghost->position.y][ghost->position.x] = ' ';
    ghost->position.x = 25;
    map[ghost->position.y][ghost->position.x] = M_GHOST;
  } else if (nextSymbol == M_RIGHT_PORTAL) {
    map[ghost->position.y][ghost->position.x] = ' ';
    ghost->position.x = 1;
    map[ghost->position.y][ghost->position.x] = M_GHOST;
  } else if (nextSymbol != M_WALL) {
    map[ghost->position.y][ghost->position.x] = ghost->prevSymbol;
    ghost->position.x += ghost->direction.x;
    ghost->position.y += ghost->direction.y;
    if (nextSymbol != M_GHOST && nextSymbol != M_PLAYER) {
      ghost->prevSymbol = nextSymbol;
    }
    map[ghost->position.y][ghost->position.x] = M_GHOST;
  }
}

// Отрисовка всех призраков
void renderGhosts(Game *game) {
  for (int i = 0; i < 4; i++) {
    Ghost *ghost = game->ghosts[i];
    wattron(game->win, COLOR_PAIR(1));
    mvwaddstr(game->win, ghost->position.y, ghost->position.x, GM_GHOST);
    wattroff(game->win, COLOR_PAIR(1));
  }
}
