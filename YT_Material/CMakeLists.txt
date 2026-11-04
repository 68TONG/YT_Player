cmake_minimum_required(VERSION 3.30)

project(YT_Material VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml ShaderTools)
qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(ExternalCode/material-color-utilities)
add_subdirectory(ExternalCode/m3shapes)

include(FetchContent)
set(QWINDOWKIT_BUILD_STATIC ON CACHE BOOL "Build QWindowKit as static library" FORCE)
set(QWINDOWKIT_BUILD_WIDGETS OFF CACHE BOOL "Build QWindowKit Widgets module" FORCE)
set(QWINDOWKIT_BUILD_QUICK ON CACHE BOOL "Don't build QWindowKit Quick module" FORCE)
set(QWINDOWKIT_BUILD_EXAMPLES OFF CACHE BOOL "Don't build examples" FORCE)
set(QWINDOWKIT_INSTALL OFF CACHE BOOL "Install QWindowKit" FORCE)
FetchContent_Declare(
    QWindowKit
    GIT_TAG main
    GIT_REPOSITORY https://github.com/stdware/qwindowkit.git
)
FetchContent_MakeAvailable(QWindowKit)

file(GLOB_RECURSE PROJECT_FILE CONFIGURE_DEPENDS
    "Quick/*"
    "Code/*"
)
file(GLOB PROJECT_ROOT_FILE "*.qml" "*.h" "*.cpp" "*.c" "*.cc")
list(APPEND PROJECT_FILE ${PROJECT_ROOT_FILE})

set(FILTER_FILE ${PROJECT_FILE})
##QML_FILE
list(FILTER FILTER_FILE INCLUDE REGEX "\\.qml$")
foreach(filePath ${FILTER_FILE})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" fileName ${filePath})
    list(APPEND PROJECT_QML_FILE ${fileName})
endforeach(filePath)
##CXX_FILE
set(PROJECT_SOURCE_FILE ${PROJECT_FILE})
list(FILTER PROJECT_SOURCE_FILE INCLUDE REGEX "\\.h$|\\.cpp$|\\.c$|\\.cc$")

set(YT_MATERIAL_QML_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}")
set(YT_MATERIAL_QML_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt6/qml")

qt_add_library(${PROJECT_NAME} "SHARED") # SHARED STATIC
target_compile_definitions(${PROJECT_NAME} PRIVATE YT_BUILD_SHARED)

qt_add_qml_module(
    ${PROJECT_NAME} VERSION 1.0
    URI 
        ${PROJECT_NAME}
    SOURCES 
        ${PROJECT_SOURCE_FILE}
        "${CMAKE_CURRENT_SOURCE_DIR}/ExternalCode/m3shapes/include/m3shapes/qml/materialshapeitem.hpp"
    QML_FILES 
        ${PROJECT_QML_FILE}
    IMPORT_PATH
        ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDENCIES 
        QtQml
        QtQuick
    OUTPUT_DIRECTORY 
        ${YT_MATERIAL_QML_OUTPUT_DIR}
)

file(GLOB_RECURSE PROJECT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Resources/Fonts/*")
qt6_add_resources(
    ${PROJECT_NAME} 
        "resources"
    FILES
        ${PROJECT_RESOURCES} 
)

file(GLOB_RECURSE PROJECT_SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Resources/Shaders/*")
qt6_add_shaders(
    ${PROJECT_NAME} 
        "shaders"
    FILES
        ${PROJECT_SHADERS} 
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Code>
    $<INSTALL_INTERFACE:include/Code>
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml

    QWindowKit::Quick
    material-color-utilities
    m3shapes
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME}plugin
    ARCHIVE DESTINATION ${YT_MATERIAL_QML_INSTALL_DIR}/${PROJECT_NAME}
    LIBRARY DESTINATION ${YT_MATERIAL_QML_INSTALL_DIR}/${PROJECT_NAME}
    RUNTIME DESTINATION ${YT_MATERIAL_QML_INSTALL_DIR}/${PROJECT_NAME}
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Code"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
        PATTERN "*.h"
)
install(
    DIRECTORY
        ${YT_MATERIAL_QML_OUTPUT_DIR}
    DESTINATION
        ${YT_MATERIAL_QML_INSTALL_DIR}
    FILES_MATCHING
        PATTERN "*.qml"
        PATTERN "*.qmltypes"
        PATTERN "qmldir"
)

# 生成配置文件
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION 
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    PATH_VARS
        YT_MATERIAL_QML_INSTALL_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)