cmake_minimum_required(VERSION 3.26)
project(Proyecto2GCRaycasting)

set(CMAKE_CXX_STANDARD 23)

add_executable(Proyecto2GCRaycasting scripts/main.cpp
        scripts/color.h
        scripts/print.h
        scripts/raycaster.h
        scripts/imageLoader.h
)

# --- SDL2 SETUP ---
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "SDL2/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "SDL2/x86_64-w64-mingw32")

find_package(SDL2_image REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY})
