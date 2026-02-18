cmake_minimum_required(VERSION 3.20)
project(TerminalMarket LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake where to look for your .hpp files
include_directories(include)

# Gather all your source files
set(SOURCES
        src/main.cpp
        src/network.cpp
        src/parser.cpp
        src/ui.cpp
        include/network.hpp
)

add_executable(terminal_markets ${SOURCES}
        include/network.hpp
        include/parser.hpp
        include/ui.hpp
)

# This is where you will eventually link libraries like CPR or FTXUI
# target_link_libraries(terminal-markets PRIVATE cpr::cpr ftxui::screen)