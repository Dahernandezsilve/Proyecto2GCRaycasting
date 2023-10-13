cmake_minimum_required(VERSION 3.26)
project(Proyecto2GCRaycasting)

set(CMAKE_PREFIX_PATH "C:/Libs/SDL2" ${CMAKE_PREFIX_PATH})
set(SDL_MAIN_HANDLED ON)
find_package(SDL2 REQUIRED)
set(EXECUTABLE_NAME "Proyecto2GCRaycasting")

set(CMAKE_CXX_STANDARD 23)

add_executable(Proyecto2GCRaycasting main.cpp
        color.h
        print.h
        raycaster.h
        imageLoader.h
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
target_include_directories(Proyecto2GCRaycasting PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(Proyecto2GCRaycasting PRIVATE ${SDL2_LIBRARIES} ${CMAKE_SOURCE_DIR}/lib/SDL2_image.dll)
