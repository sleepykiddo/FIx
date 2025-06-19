cmake_minimum_required(VERSION 3.10)

project(ColorWarSDL)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(ColorWarSDL
    src/main.cpp
    src/GameState.cpp
    src/Cell.cpp
)

target_link_libraries(ColorWarSDL PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

project(ColorWarSDL)

if(MINGW)
    set(CMAKE_CXX_COMPILER "g++")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_executable(ColorWarSDL
    src/main.cpp
    src/GameState.cpp
    src/Cell.cpp
)

target_include_directories(ColorWarSDL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ColorWarSDL PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})

# Enable warnings (optional but recommended)
if(MSVC)
    target_compile_options(ColorWarSDL PRIVATE /W3)
else()
    target_compile_options(ColorWarSDL PRIVATE -Wall -Wextra -pedantic)
endif()

install(TARGETS ColorWarSDL DESTINATION bin)

# Suggestion for user: Ensure Arial.ttf is available
# You might need to copy Arial.ttf to the build directory or specify its path in the code.
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
message(STATUS "SDL2_TTF_LIBRARIES: ${SDL2_TTF_LIBRARIES}")
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_TTF_INCLUDE_DIRS: ${SDL2_TTF_INCLUDE_DIRS}")