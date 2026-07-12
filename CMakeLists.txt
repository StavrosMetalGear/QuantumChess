cmake_minimum_required(VERSION 3.16)

project(QuantumChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(quantum_chess_core
    src/core/Board.cpp
    src/core/Game.cpp
    src/core/QuantumPiece.cpp
)

target_include_directories(quantum_chess_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(QuantumChessCLI
    src/cli/main.cpp
)

target_link_libraries(QuantumChessCLI
    PRIVATE
        quantum_chess_core
)
