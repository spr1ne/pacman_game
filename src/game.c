#include "game.h"
#include "game_menu.h"
#include "ghost.h"
#include "player.h"
#include <string.h>
#include <syslog.h>
#include <unistd.h>

char map[21][28] = {
    "###########################", "#............#............#",
    "#O####.#####.#.####.#####O#", "#.........................#",
    "#.####.#.#########.#.####.#", "#......#.....#.....#......#",
    "######.#####.#.#####.######", "     #.#           #.#     ",
    "######.#           #.######", "[     .             .     ]",
    "######.#           #.######", "     #.#           #.#     ",
    "######.#..#######..#.######", "#............#............#",
    "#.####.#####.#.#####.####.#", "#O...#.......<.......#...O#",
    "####.#.#.#########.#.#.####", "#......#.....#.....#......#",
    "#.####.#####.#.#####.####.#", "#.........................#",
    "###########################"};

// Префикс M_ - то как объекты обозначаются в объекте map
// Префикс GM_ - то как объекты отображаются на экране
const char M_PLAYER = '<';
const char *GM_PLAYER = "ᗤ";

const char M_GHOST = 'O';
const char *GM_GHOST = "ᗣ";

const char M_WALL = '#';

const char M_FOOD = '.';
const char *GM_FOOD = "•";

const char M_LEFT_PORTAL = '[';
const char M_RIGHT_PORTAL = ']';

Direction DIRECTION_NONE = {.x = 0, .y = 0};
Direction DIRECTION_LEFT = {.x = -1, .y = 0};
Direction DIRECTION_RIGHT = {.x = 1, .y = 0};
Direction DIRECTION_UP = {.x = 0, .y = -1};
Direction DIRECTION_DOWN = {.x = 0, .y = 1};

// Функция отрисовки карты
void renderMap(Game *game) {
  int rowsCount = ARRAY_SIZE(map);
  int colsCount = ARRAY_SIZE(map[0]);
  char symbol;

  for (int y = 0; y < rowsCount; y++) {
    for (int x = 0; x < colsCount; x++) {
      symbol = map[y][x];

      if (symbol == '#') {
        mvwaddch(game->win, y, x, symbol);
      } else if (symbol == '[') {
        mvwaddch(game->win, y, x, symbol);
      } else if (symbol == ']') {
        mvwaddch(game->win, y, x, symbol);
      } else if (symbol == '.') {
        mvwaddstr(game->win, y, x, GM_FOOD);
      } else if (symbol == ' ') {
        mvwaddch(game->win, y, x, symbol);
      }
    }
  }
}

// Полькователь победил?
bool isWin(Game *game) { return game->foodCount == 0; };

// Пользователь проиграл?
bool isLoose(Game *game) { return game->player->lives == 0; }

// Получение следующего символа на карте в зависимости от направления игрока
char getNextSymbol(Game *game, Position *playerPosition, Direction *direction) {

  const int nextPosX = playerPosition->x + direction->x;
  const int nextPosY = playerPosition->y + direction->y;

  const char nextSymbol = map[nextPosY][nextPosX];

  return nextSymbol;
}

// Обновление модели игры в зависимости от направления движения
void updateModel(Game *game, Direction direction, DirectionName directionName) {

  int nextPosX = game->player->position.x + direction.x;
  int nextPosY = game->player->position.y + direction.y;

  const char symbol = map[nextPosY][nextPosX];

  if (symbol != '#') {
    game->player->direction.x = direction.x;
    game->player->direction.y = direction.y;

    if (directionName == LEFT) {
      strcpy(game->player->icon, "ᗤ");
    } else if (directionName == DOWN) {
      strcpy(game->player->icon, "ᗣ");
    } else if (directionName == UP) {
      strcpy(game->player->icon, "ᗢ");
    } else if (directionName == RIGHT) {
      strcpy(game->player->icon, "ᗧ");
    } else if (directionName == NONE) {
      strcpy(game->player->icon, "ᗤ");
    }
  }

  updatePlayer(game);
  checkCollision(game);

  for (int i = 0; i < 4; i++) {
    Ghost *ghost = game->ghosts[i];

    DirectionName nextDirection = getNextGhostDirection(game, ghost);
    ghost->directionName = nextDirection;

    switch (nextDirection) {
    case LEFT:
      ghost->direction = DIRECTION_LEFT;
      break;
    case UP:
      ghost->direction = DIRECTION_UP;
      break;
    case RIGHT:
      ghost->direction = DIRECTION_RIGHT;
      break;
    case DOWN:
      ghost->direction = DIRECTION_DOWN;
      break;
    }

    updateGhost(game, game->ghosts[i]);
    checkCollision(game);
  };
}

// Проверка на столкновения между игроком и призраками
void checkCollision(Game *game) {
  Player *player = game->player;
  const int pX = player->position.x;
  const int pY = player->position.y;
  bool isCollide = false;

  for (int i = 0; i < 4; i++) {
    Ghost *ghost = game->ghosts[i];
    const int gX = ghost->position.x;
    const int gY = ghost->position.y;

    // Проверка, находится ли игрок на одной позиции с призраком
    if (gX == pX && gY == pY) {
      isCollide = true;
      break;
    }
  }

  // Если произошло столкновение, уменьшение жизней игрока
  if (isCollide && game->player->lives > 0) {
    game->player->lives -= 1;
    // Возврат игрока на стартовую позицию
    map[game->player->position.y][game->player->position.x] = ' ';
    game->player->position.y = 15;
    game->player->position.x = 13;
  }
};

// Отрисовка статистики игрока (количество еды и жизней)
void renderStats(Game *game) {
  mvwprintw(game->stats_win, 1, 1, "Еда: %d", game->foodCount);

  mvwprintw(game->stats_win, 1, 18, "Жизни: %d", game->player->lives);
}

// Основная функция отрисовки всех элементов игры
void render(Game *game) {
  renderMap(game);
  renderPlayer(game);
  renderGhosts(game);
  renderStats(game);

  refresh();
  wrefresh(game->win);
  wrefresh(game->stats_win);
}

// Основной игровой цикл
void game_loop(Game *game, const char *choices[]) {

  DirectionName directionName = NONE;
  Direction playerDirection = DIRECTION_NONE;
  MM_RESULT selected_menu_item = NOOP;

  int c;
  int loop = true;

  while (loop) {

    // Проверка условий завершения игры
    if (isWin(game) || isLoose(game)) {
      loop = false;
      break;
    }

    // Получение нажатой клавиши
    c = wgetch(game->win);

    switch (c) {
      // ESC для открытия меню
    case 27: {
      clear();
      MM_WINDOW *mm_window = create_menu(choices);
      selected_menu_item = menu_start(mm_window);
    } break;
    case KEY_LEFT:
      playerDirection = DIRECTION_LEFT;
      directionName = LEFT;
      break;
    case KEY_UP:
      playerDirection = DIRECTION_UP;
      directionName = UP;
      break;
    case KEY_DOWN:
      playerDirection = DIRECTION_DOWN;
      directionName = DOWN;
      break;
    case KEY_RIGHT:
      playerDirection = DIRECTION_RIGHT;
      directionName = RIGHT;
      break;
    }

    if (selected_menu_item == EXIT) {
      loop = false;
      break;
    }

    // Обновление модели игры
    updateModel(game, playerDirection, directionName);

    // Отрисовка по модели
    render(game);

    // Пауза до сл. цикла
    usleep(300000);
  }
}

// Создание игрового окна (где будет отрисована карта)
WINDOW *create_game_win() {
  const int width = 28;
  const int height = 21;

  int maxRows, maxCols;
  getmaxyx(stdscr, maxRows, maxCols);

  syslog(LOG_USER, "Pacman: MaxRows %d, MaxCols %d", maxRows, maxCols);

  WINDOW *win =
      newwin(height, width, (maxRows - height) / 2, (maxCols - width) / 2);
  keypad(win, TRUE);
  wtimeout(win, 0);

  return win;
}

// Создание окна статистики
WINDOW *create_stats_win(WINDOW *game_win) {
  const int width = 28;
  const int height = 3;

  int x, y;
  getbegyx(game_win, y, x);

  WINDOW *win = newwin(height, width, y - height, x);
  box(win, 0, 0);

  refresh();
  wrefresh(win);

  return win;
}

// Создание диалогового окна с сообщением
WINDOW *create_dialog(const char *message) {
  const int width = 21;
  const int height = 5;
  int maxRows, maxCols;
  getmaxyx(stdscr, maxRows, maxCols);
  WINDOW *win =
      newwin(height, width, (maxRows - height) / 2, (maxCols - width) / 2);
  keypad(win, TRUE);
  wtimeout(win, 0);

  box(win, 0, 0);

  const int message_length = (int)strlen(message) / 2;
  mvwaddstr(win, height / 2, (width - message_length) / 2, message);

  refresh();
  wrefresh(win);

  return win;
}
