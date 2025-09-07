cmake_minimum_required(VERSION 3.16)

project(YT_Player VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include(FetchContent)
# FetchContent_Declare(
#   SDL2_DLL
#   GIT_REPOSITORY "https://github.com/68TONG/SDL2_DLL.git"
#   GIT_TAG "main"
# )
# FetchContent_MakeAvailable(SDL2_DLL)

# include(ExternalProject)
# ExternalProject_Add(
#     SDL2_DLL_download
#     GIT_REPOSITORY "https://github.com/68TONG/SDL2_DLL.git"
#     GIT_TAG        "main"
#     SOURCE_DIR     "${CMAKE_BINARY_DIR}/SDL2_DLL"
#     CONFIGURE_COMMAND ""
#     BUILD_COMMAND     ""
#     INSTALL_COMMAND   ""
# )

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory("MediaOperations" "MediaOperations")

include_directories(
    "YT_CXX"
    "YT_Quick"
    "YT_Quick/NodeEditor"
    "MainWidget/FileWidget"
    "MainWidget/MusicWidget"
    "MainWidget/ControlWidget"
)

file(GLOB_RECURSE PROJECT_FILE "YT_CXX/*" "YT_Quick/*" "MainWidget/*")
##QML_FILE
set(FILTER_FILE ${PROJECT_FILE})
list(FILTER FILTER_FILE INCLUDE REGEX "\\.qml$")
foreach(filePath ${FILTER_FILE})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" fileName ${filePath})
    list(APPEND PROJECT_QML_FILE ${fileName})
endforeach(filePath)
##CXX_FILE
set(PROJECT_SOURCE_FILE ${PROJECT_FILE})
list(FILTER PROJECT_SOURCE_FILE INCLUDE REGEX "\\.h$|\\.cpp$")
##RESOURCE_FILE
file(GLOB_RECURSE PROJECT_RESOURCE_FILE "")

set_source_files_properties("MainWidget/ControlWidget/YT.qml" PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_executable(${PROJECT_NAME})
qt_add_qml_module( ${PROJECT_NAME}
    VERSION 1.0
    URI ${PROJECT_NAME}
    QML_FILES "Main.qml" ${PROJECT_QML_FILE}
    RESOURCES "Resource/Resource_UI.qrc"
    SOURCES "main.cpp" ${PROJECT_SOURCE_FILE} ${PROJECT_RESOURCE_FILE}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Multimedia)
target_link_libraries(${PROJECT_NAME} PRIVATE MediaOperations)
target_include_directories(${PROJECT_NAME} PRIVATE MediaOperations)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
