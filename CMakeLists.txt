cmake_minimum_required(VERSION 3.10)
project(BlackJackVisual)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Busca SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Archivo fuente
add_executable(BlackJack main.cpp)

# Vincular con SFML
target_link_libraries(BlackJack sfml-graphics sfml-window sfml-system)

# Copiar assets automáticamente al build (opcional pero útil)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
