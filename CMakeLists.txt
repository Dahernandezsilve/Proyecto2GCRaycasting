cmake_minimum_required(VERSION 3.26)
project(Proyecto2GCRaycasting)

set(CMAKE_PREFIX_PATH "C:/Libs/SDL2" ${CMAKE_PREFIX_PATH})
set(SDL_MAIN_HANDLED ON)
find_package(SDL2 REQUIRED)
set(SDL2_IMAGE_PATH "SDL2/x86_64-w64-mingw32")
find_package(SDL2_image REQUIRED)
set(EXECUTABLE_NAME "Proyecto2GCRaycasting")

set(CMAKE_CXX_STANDARD 23)

add_executable(Proyecto2GCRaycasting main.cpp
        color.h
        print.h
        raycaster.h
        imageLoader.h
)
# Añadir la ruta a las cabeceras de SDL_image

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
target_include_directories(Proyecto2GCRaycasting PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(Proyecto2GCRaycasting PRIVATE ${SDL2_LIBRARIES})
