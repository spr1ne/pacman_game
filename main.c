#include "src/game.h"
#include "src/game_menu.h"
#include "src/ghost.h"
#include "src/player.h"
#include <curses.h>
#include <locale.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Инициализация объектов на карте
void init_game(Game *game) {
  // Количество приведений
  const int ghosts_count = 4;
  // Выделяем память для привидений.
  game->ghosts = (Ghost **)calloc((size_t)ghosts_count, sizeof(Ghost *));

  // Инициализируем каждое привидение.
  for (int i = 0; i < 4; i++) {
    game->ghosts[i] = (Ghost *)calloc(1, sizeof(Ghost));
    game->ghosts[i]->id = i;
  }

  int ghostIdx = 0;
  // Количество строк на карте
  int rowsCount = ARRAY_SIZE(map);
  // Количество колонок на карте
  int colsCount = ARRAY_SIZE(map[0]);

  for (int y = 0; y < rowsCount; y++) {
    for (int x = 0; x < colsCount; x++) {
      const char symbol = map[y][x];

      bool isPlayer = symbol == M_PLAYER;
      bool isGhost = symbol == M_GHOST;
      bool isFood = symbol == M_FOOD;

      // Символ обозначает игрока
      if (isPlayer) {
        game->player = (Player *)calloc(1, sizeof(Player));
        game->player->position.x = x;
        game->player->position.y = y;
        game->player->direction.x = 0;
        game->player->direction.y = 0;
        game->player->lives = 10;
        strcpy(game->player->icon, GM_PLAYER);
        // Символ обозначает врага
      } else if (isGhost) {
        Ghost *ghost = game->ghosts[ghostIdx];
        ghost->position.x = x;
        ghost->position.y = y;

        int randNum = RANDOM_BETWEEN(0, 3);

        if (randNum == 0) {
          ghost->direction = DIRECTION_LEFT;
          ghost->directionName = LEFT;
        } else if (randNum == 1) {
          ghost->direction = DIRECTION_RIGHT;
          ghost->directionName = RIGHT;
        } else if (randNum == 2) {
          ghost->direction = DIRECTION_UP;
          ghost->directionName = UP;
        } else if (randNum == 3) {
          ghost->direction = DIRECTION_DOWN;
          ghost->directionName = DOWN;
        }

        ghost->immortal = false;
        ghost->prevSymbol = M_FOOD;
        strcpy(ghost->icon, GM_GHOST);
        ghostIdx++;
      }
    }
  }

  render(game);
}

int main(void) {
  setlocale(LC_ALL, "");
  srand(time(NULL));

  // Элементы меню
  const char *choices[] = {
      MM_START,
      MM_EXIT,
      (char *)NULL,
  };

  // Инициализация ncurses
  initscr();
  // Включение поддержки цвета
  start_color();
  // Не дожидаемся пока пользователь нажмет Enter
  cbreak();
  // Не отображаем символы введенные с клавиатуры
  noecho();
  // Скрываем курсор
  curs_set(0);
  // Включем обработку с клавиатуры (стрелки и.т.п)
  keypad(stdscr, TRUE);

  // Инициализация цветовых пар
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_YELLOW, COLOR_BLACK);

  // Храним на стеке, т.к данных немного
  Game game = {.foodCount = 214};
  game.win = create_game_win();

  // Создаем главное меню игры
  MM_WINDOW *mm_window = create_menu(choices);

  if (!mm_window) {
    printf("Не удалось выделить память под окно mm_window");
    exit(1);
  }

  // Отображаем окно, и ожидаем выбор пользователя
  MM_RESULT selected_menu_item = menu_start(mm_window);

  // Если пользователь выбрал "Начать"
  if (selected_menu_item == START) {
    // Создаем окно статистики (Кол-во еды, кол-во жизней)
    game.stats_win = create_stats_win(game.win);
    // Инициализация игры
    init_game(&game);
    // Старт основного цикла игры
    game_loop(&game, choices);

    // Пользователь выйграл? (Если да отображаем диалоговое окно)
    if (isWin(&game)) {
      WINDOW *win = create_dialog("Вы выйграли!");
      getch();
      destroy_win(win);
    }

    // Пользователь проиграл? (Если да отображаем диалоговое окно)
    if (isLoose(&game)) {
      WINDOW *win = create_dialog("Вы проиграли!");
      getch();
      destroy_win(win);
    }
  }

  // Освобождение памяти
  free(game.ghosts);

  // Завершение ncurses
  endwin();
  // Очищение экрана
  system("clear");
  return 0;
}
