cmake_minimum_required(VERSION 3.5)

project(MediaOperations VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROJECT_FILE "*.h" "*.cpp")

add_library(${PROJECT_NAME} "STATIC" ${PROJECT_FILE})

#----------------------------------------------------------

set(LIBRARY_DIR "C:/TONG/IT")
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    ################################################## Windows FFmpeg
    set(FFMPEG_LIBRARY_PATH "${LIBRARY_DIR}/FFmpeg/windows/lib")
    set(
        FFMPEG_LIBRARY
        "${FFMPEG_LIBRARY_PATH}/avcodec.lib"
        "${FFMPEG_LIBRARY_PATH}/avfilter.lib"
        "${FFMPEG_LIBRARY_PATH}/avformat.lib"
        "${FFMPEG_LIBRARY_PATH}/avutil.lib"
        "${FFMPEG_LIBRARY_PATH}/swresample.lib"
        "${FFMPEG_LIBRARY_PATH}/swscale.lib"
        CACHE FILEPATH "FFMPEG_LIBRARY"
    )

    ################################################## Windows SDL2
    set(SDL2_LIBRARY "${LIBRARY_DIR}/SDL2/windows/SDL2.dll" CACHE FILEPATH "SDL2_LIBRARY")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    ################################################## Android FFmpeg
    set(FFMPEG_LIBRARY_PATH "${LIBRARY_DIR}/FFmpeg/android/arm64-v8a")
    set(
        FFMPEG_LIBRARY
        "${FFMPEG_LIBRARY_PATH}/libavcodec.so"
        "${FFMPEG_LIBRARY_PATH}/libavfilter.so"
        "${FFMPEG_LIBRARY_PATH}/libavformat.so"
        "${FFMPEG_LIBRARY_PATH}/libavutil.so"
        "${FFMPEG_LIBRARY_PATH}/libswresample.so"
        "${FFMPEG_LIBRARY_PATH}/libswscale.so"
        CACHE FILEPATH "FFMPEG_LIBRARY"
    )

    ################################################## Android SDL2
    set(SDL2_LIBRARY "${LIBRARY_DIR}/SDL2/android/arm64-v8a/libSDL2.so" CACHE FILEPATH "SDL2_LIBRARY")
else()
    message(STATUS "Target System Is Unknown")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/General")

target_link_libraries(${PROJECT_NAME} PUBLIC "${SDL2_LIBRARY}")
target_include_directories(${PROJECT_NAME} PUBLIC "${LIBRARY_DIR}/SDL2/include")

target_link_libraries(${PROJECT_NAME} PUBLIC "${FFMPEG_LIBRARY}")
target_include_directories(${PROJECT_NAME} PUBLIC "${LIBRARY_DIR}/FFmpeg/include")
