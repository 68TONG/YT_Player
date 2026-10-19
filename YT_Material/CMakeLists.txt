cmake_minimum_required(VERSION 3.30)

project(YT_Material VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(ABSL_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ABSL_USE_GOOGLETEST_HEAD OFF CACHE BOOL "" FORCE)
set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    abseil
    GIT_TAG lts_2026_05_26
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
)
FetchContent_MakeAvailable(abseil)

set(QWINDOWKIT_BUILD_STATIC OFF CACHE BOOL "Build QWindowKit as static library" FORCE)
set(QWINDOWKIT_BUILD_WIDGETS OFF CACHE BOOL "Build QWindowKit Widgets module" FORCE)
set(QWINDOWKIT_BUILD_QUICK ON CACHE BOOL "Don't build QWindowKit Quick module" FORCE)
set(QWINDOWKIT_BUILD_EXAMPLES OFF CACHE BOOL "Don't build examples" FORCE)
set(QWINDOWKIT_INSTALL ON CACHE BOOL "Don't install QWindowKit" FORCE)
FetchContent_Declare(
    QWindowKit
    GIT_TAG main
    GIT_REPOSITORY https://github.com/stdware/qwindowkit.git
)
FetchContent_MakeAvailable(QWindowKit)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml ShaderTools)
qt_standard_project_setup(REQUIRES 6.8)

file(GLOB_RECURSE PROJECT_FILE 
    "ExternalCode/*"
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
set(YT_MATERIAL_QML_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qml")

set_source_files_properties("Quick/AnimationTemplate.qml" PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_library(${PROJECT_NAME} "SHARED") # SHARED STATIC
target_compile_definitions(${PROJECT_NAME} PRIVATE YT_BUILD_SHARED)

qt_add_qml_module(
    ${PROJECT_NAME} VERSION 1.0
    URI 
        ${PROJECT_NAME}
    SOURCES 
        ${PROJECT_SOURCE_FILE}
    QML_FILES 
        ${PROJECT_QML_FILE}
    DEPENDENCIES 
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

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_QML_IMPORT_PATH "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Code>
    $<INSTALL_INTERFACE:include/Code>

    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ExternalCode/material-color-utilities>
    $<INSTALL_INTERFACE:include/ExternalCode/material-color-utilities>
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    absl::strings
    absl::flat_hash_map
)
target_link_libraries(${PROJECT_NAME} PUBLIC
    QWindowKit::Quick
)

include(GNUInstallDirs)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Code"  "${CMAKE_CURRENT_SOURCE_DIR}/ExternalCode"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
        PATTERN "*.h"
)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# 生成配置文件
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