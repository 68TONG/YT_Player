cmake_minimum_required(VERSION 3.30)

project(MediaOperations VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROJECT_FILE "*.h" "*.cpp")
add_library(${PROJECT_NAME} "STATIC" ${PROJECT_FILE})

#----------------------------------------------------------

# include(FetchContent)
# FetchContent_Declare(FFmpeg_Project GIT_REPOSITORY "https://github.com/68TONG/FFmpeg.git" GIT_TAG "main")
# FetchContent_MakeAvailable(FFmpeg_Project)

if(NOT DEFINED YT_Library)
    set(YT_Library "C:/TONG/IT")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB FFmpeg_Library "${YT_Library}/FFmpeg/windows/bin/*.dll")
    set(MediaOperationsLibrary ${FFmpeg_Library} PARENT_SCOPE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Target System Is Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    message("Target System Is Android")
else()
    message("Target System Is Unknown")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/General")

target_link_libraries(${PROJECT_NAME} PUBLIC "${FFmpeg_Library}")
target_include_directories(${PROJECT_NAME} PUBLIC "${YT_Library}/FFmpeg/include")
