#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "game_menu.h"
#include <menu.h>
#include <stdlib.h>
#include <string.h>

// Удаление окна
void destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  werase(local_win);
  refresh();
  wrefresh(local_win);
  keypad(local_win, FALSE);
  delwin(local_win);
}

// Создание заголовка для окна (с центрированием)
void create_menu_header(WINDOW *win, const char *string, chtype color) {
  int length, x, y, my, mx;
  float center;

  // Получаем начальные координаты окна
  getyx(win, y, x);

  // Получаем максимальные координаты окна
  getmaxyx(win, my, mx);

  // Длина строки
  length = (int)strlen(string);

  // Центр строки
  center = (float)(mx - length) / 2;
  x = x + (int)center;

  // Применяем цвет к тексту
  wattron(win, color);
  mvwprintw(win, y + 1, x, "%s", string);
  wattroff(win, color);

  // Добавляем символ вертикальной черты (левый)
  mvwaddch(win, 2, 0, ACS_LTEE);

  // Добавляем горизонтальную линию
  mvwhline(win, 2, 1, ACS_HLINE, 38);

  // Добавляем символ вертикальный черты (правый)
  mvwaddch(win, 2, 39, ACS_RTEE);
}

// Создание основного меню (MM - Main Menu)
MM_WINDOW *create_menu(const char *choices[]) {
  const int width = 40;
  const int height = 10;

  int maxRows, maxCols;
  getmaxyx(stdscr, maxRows, maxCols);

  MM_WINDOW *mm_window = (MM_WINDOW *)calloc(1, sizeof(MM_WINDOW));
  /* Создание элементов меню */
  int n_choices = 3;

  // Выделение памяти для элементов меню
  mm_window->my_items = (ITEM **)calloc((size_t)(n_choices), sizeof(ITEM *));
  for (int i = 0; i < n_choices; ++i)
    mm_window->my_items[i] = new_item(choices[i], NULL);

  /* Создание меню */
  mm_window->my_menu = new_menu((ITEM **)mm_window->my_items);

  /* Создание окна для меню */
  mm_window->main_menu_win =
      newwin(height, width, (maxRows - height) / 2, (maxCols - width) / 2);
  keypad(mm_window->main_menu_win, TRUE);

  /* Создание основного окна и дочернего окна */
  set_menu_win(mm_window->my_menu, mm_window->main_menu_win);
  set_menu_sub(mm_window->my_menu,
               derwin(mm_window->main_menu_win, 6, 38, 3, 1));

  /* Отключаем символ - для текущего выделенного элемента */
  set_menu_mark(mm_window->my_menu, "");

  /* Добавляем рамку вокруг окна */
  box(mm_window->main_menu_win, 0, 0);

  /* Создание заголовка */
  create_menu_header(mm_window->main_menu_win, "Pacman", COLOR_PAIR(5));
  refresh();

  /* Опубликовать меню */
  post_menu(mm_window->my_menu);
  wrefresh(mm_window->main_menu_win);

  return mm_window;
}

// Отображаем окно основного меню, и ожидаем выбор пользователя
MM_RESULT menu_start(MM_WINDOW *mm_window) {

  int n_choices = 3;
  int c;
  const char *menu_item_selected = "";
  bool isExit = false;
  bool isStart = false;
  bool loop = true;

  // Ожидаем выбор пользователя
  while (loop) {
    isExit = strcmp(menu_item_selected, MM_EXIT) == 0;
    isStart = strcmp(menu_item_selected, MM_START) == 0;

    if (isExit || isStart) {
      loop = false;
      break;
    }

    // Читаем ввод с клавиатуры
    c = wgetch(mm_window->main_menu_win);

    switch (c) {
    case KEY_DOWN:
      menu_driver(mm_window->my_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(mm_window->my_menu, REQ_UP_ITEM);
      break;
    case 10:
      menu_item_selected = item_name(current_item(mm_window->my_menu));
      break;
    }

    wrefresh(mm_window->main_menu_win);
  }

  if (isStart || isExit) {
    menu_item_selected = NULL;
    // Освобождение памяти (меню)
    unpost_menu(mm_window->my_menu);
    free_menu(mm_window->my_menu);
    destroy_win(mm_window->main_menu_win);

    for (int i = 0; i < n_choices; ++i)
      free_item(mm_window->my_items[i]);

    // Очистка экрана
    clear();
    refresh();
  }

  if (isStart) {
    return START;
  }

  if (isExit) {
    return EXIT;
  }

  return ERROR;
};

#endif
