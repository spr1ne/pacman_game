set(CMAKE_DEBUG_POSTFIX _debug)
cmake_minimum_required(VERSION 3.28)
project(lab_kr)

set(CMAKE_C_STANDARD 17)
add_compile_options(-D_XOPEN_SOURCE=600 -O0)

add_executable(lab_kr main.c 
  src/game_menu.c 
  src/player.c 
  src/ghost.c 
  src/game.c
)
set_target_properties(lab_kr PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(lab_kr "ncursesw" "menuw")
