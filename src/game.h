#ifndef GAME_H_
#define GAME_H_
#include <menu.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define RANDOM_BETWEEN(min, max) (rand() % (max - min + 1) + min)

extern char map[21][28];

extern const char M_PLAYER;
extern const char *GM_PLAYER;

extern const char M_GHOST;
extern const char *GM_GHOST;

extern const char M_WALL;

extern const char M_FOOD;
extern const char *GM_FOOD;

extern const char M_LEFT_PORTAL;
extern const char M_RIGHT_PORTAL;

typedef struct Player Player;
typedef struct Ghost Ghost;
typedef struct Direction Direction;

extern Direction DIRECTION_NONE;
extern Direction DIRECTION_LEFT;
extern Direction DIRECTION_RIGHT;
extern Direction DIRECTION_UP;
extern Direction DIRECTION_DOWN;

// x и y не могут иметь одновременно значение больше 0 (игроки не могут
// двигаться по диагонали)
struct Direction {
  // Может иметь значение -1, 1
  int x;
  // Может иметь значение -1, 1
  int y;
};

typedef enum { NONE = -2, LEFT = 0, UP = 1, DOWN = 2, RIGHT = 3 } DirectionName;

typedef struct Position Position;
struct Position {
  int x;
  int y;
};

typedef struct Game Game;
struct Game {
  int foodCount;
  WINDOW *win;
  WINDOW *stats_win;
  Player *player;
  Ghost **ghosts;
};

void checkCollision(Game *game);

bool isWin(Game *game);

bool isLoose(Game *game);

char getNextSymbol(Game *game, Position *playerPosition, Direction *direction);

void renderMap(Game *game);

void render(Game *game);
void game_loop(Game *game, const char *choices[]);
WINDOW *create_game_win();
WINDOW *create_stats_win(WINDOW *game_win);
WINDOW *create_dialog(const char *message);
#endif
