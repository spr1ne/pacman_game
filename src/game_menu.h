#include <menu.h>

#ifndef GAME_MENU_H_
#define GAME_MENU_H_
#define MM_START "Начать игру"
#define MM_EXIT "Выйти"

typedef enum { NOOP = -2, ERROR = -1, EXIT = 0, START = 1 } MM_RESULT;

typedef struct MM_WINDOW MM_WINDOW;
struct MM_WINDOW {
  ITEM **my_items;
  MENU *my_menu;
  WINDOW *main_menu_win;
};

void destroy_win(WINDOW *local_win);
void create_menu_header(WINDOW *win, const char *string, chtype color);
MM_WINDOW *create_menu(const char *choices[]);
MM_RESULT menu_start(MM_WINDOW *mm_window);

#endif
