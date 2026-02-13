cmake_minimum_required(VERSION 3.30)

project(YT_Player VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/ CACHE STRING "" FORCE)

find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(Qt6 REQUIRED COMPONENTS QuickControls2)
qt_standard_project_setup(REQUIRES 6.8)

set(YT_Library "C:/TONG/IT")
add_subdirectory("MediaOperations" "MediaOperations")

include_directories(
    "YT_CXX"
    "YT_Quick"
    "YT_Database"
    "YT_External"
    "YT_NodeEditor"
    "MainWidget/EditWidget"
    "MainWidget/FileWidget"
    "MainWidget/MusicWidget"
    "MainWidget/ControlWidget"
)

file(GLOB_RECURSE PROJECT_FILE 
    "YT_CXX/*" 
    "YT_Quick/*" 
    "YT_Database/*"
    "YT_External/*" 
    "YT_NodeEditor/*" 
    "MainWidget/*"
)

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
qt_add_qml_module(
    ${PROJECT_NAME} VERSION 1.0
    URI 
        ${PROJECT_NAME}
    QML_FILES 
        "Main.qml" 
        ${PROJECT_QML_FILE}
    RESOURCES 
        "Resource/Resource_UI.qrc"
    SOURCES 
        "main.cpp" 
        ${PROJECT_SOURCE_FILE} 
        ${PROJECT_RESOURCE_FILE}
    DEPENDENCIES 
        QtQuick
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE
    # WIN32_EXECUTABLE <CONFIG:Release>
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Sql)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Multimedia)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Concurrent)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::QuickControls2)

target_link_libraries(${PROJECT_NAME} PRIVATE MediaOperations)
target_include_directories(${PROJECT_NAME} PRIVATE MediaOperations)

target_link_libraries(${PROJECT_NAME} PRIVATE "${YT_Library}/chinese-util-cpp/lib/libchinese_util.a")
target_include_directories(${PROJECT_NAME} PRIVATE "${YT_Library}/chinese-util-cpp/include")

target_link_libraries(${PROJECT_NAME} PRIVATE "${YT_Library}/SQLite/sqlite3.dll")
target_include_directories(${PROJECT_NAME} PRIVATE "${YT_Library}/SQLite")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_BINARY_DIR}/YT_PlayerData/chinese_util"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${YT_Library}/chinese-util-cpp/data/charsData.json"
        "${CMAKE_BINARY_DIR}/YT_PlayerData/chinese_util/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${YT_Library}/chinese-util-cpp/data/pinyinData.json"
        "${CMAKE_BINARY_DIR}/YT_PlayerData/chinese_util/"
    COMMENT "Copying Chinese utility data files to build directory"
)

# If MediaOperations exported a list of FFmpeg DLLs (Windows), copy them to the exe output after build
if(WIN32 AND DEFINED MediaOperationsLibrary)
    foreach(_ff ${MediaOperationsLibrary})
        if(EXISTS ${_ff})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_ff}"
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying FFmpeg DLL ${_ff} to output directory"
            )
        endif()
    endforeach()
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
