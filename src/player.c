#include "player.h"

void updatePlayer(Game *game) {
  // Вычисляем следующие координаты игрока на основе его текущего положения и
  // направления.
  const int nextPosX = game->player->position.x + game->player->direction.x;
  const int nextPosY = game->player->position.y + game->player->direction.y;

  // Получаем символ, находящийся в следующей позиции на карте.
  const char nextSymbol = map[nextPosY][nextPosX];

  // Проверяем, попадает ли игрок в левый портал.
  if (nextSymbol == M_LEFT_PORTAL) {
    // Очищаем текущее положение игрока на карте.
    map[game->player->position.y][game->player->position.x] = ' ';
    // Перемещаем игрока в правый портал.
    game->player->position.x = 25;
    // Обновляем карту, устанавливая символ игрока в новой позиции.
    map[game->player->position.y][game->player->position.x] = M_PLAYER;

    // Проверяем, попадает ли игрок в правый портал.
  } else if (nextSymbol == M_RIGHT_PORTAL) {
    map[game->player->position.y][game->player->position.x] = ' ';
    game->player->position.x = 1;
    map[game->player->position.y][game->player->position.x] = M_PLAYER;

    // Проверяем, нашел ли игрок еду.
  } else if (nextSymbol == M_FOOD) {
    game->foodCount -= 1;
    map[game->player->position.y][game->player->position.x] = ' ';
    game->player->position.x += game->player->direction.x;
    game->player->position.y += game->player->direction.y;
    map[game->player->position.y][game->player->position.x] = M_PLAYER;

    // Перемещаем игрока на новую позицию если это не стена
  } else if (nextSymbol != M_WALL) {
    map[game->player->position.y][game->player->position.x] = ' ';
    game->player->position.x += game->player->direction.x;
    game->player->position.y += game->player->direction.y;
    map[game->player->position.y][game->player->position.x] = M_PLAYER;
  }
}

void renderPlayer(Game *game) {
  // Включаем цвет для отображения игрока.
  wattron(game->win, COLOR_PAIR(5));
  // Отображаем иконку игрока
  mvwaddstr(game->win, game->player->position.y, game->player->position.x,
            game->player->icon);
  // Выключаем
  wattroff(game->win, COLOR_PAIR(5));
}
