cmake_minimum_required(VERSION 3.30)

project(YT_Player VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(YT_Library "C:/TONG/IT")

# set(YT_Material_DIR "C:/TONG/Code/YT_Material/build/install/lib/cmake/YT_Material")
# find_package(YT_Material REQUIRED)

find_package(HuskarUI REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(Qt6 REQUIRED COMPONENTS ShaderTools)
find_package(Qt6 REQUIRED COMPONENTS QuickControls2)
qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory("YT_Material" "YT_Material")
add_subdirectory("MediaOperations" "MediaOperations")
include_directories(
    "YT_CXX"
    "YT_Quick"
    "YT_Database"
    "YT_External"
    "YT_NodeEditor"
    "MainWidget/PathWidget"
    "MainWidget/EditWidget"
    "MainWidget/FileWidget"
    "MainWidget/MusicWidget"
    "MainWidget/ControlWidget"
    "MainWidget/PictureWidget"
)
file(GLOB_RECURSE PROJECT_FILE 
    "YT_CXX/*" 
    "YT_Quick/*" 
    "YT_Database/*"
    "YT_External/*" 
    "YT_NodeEditor/*" 
    "MainWidget/*"
)

set(FILTER_FILE ${PROJECT_FILE})
##QML_FILE
list(FILTER FILTER_FILE INCLUDE REGEX "\\.qml$")
foreach(filePath ${FILTER_FILE})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" fileName ${filePath})
    list(APPEND PROJECT_QML_FILE ${fileName})
endforeach(filePath)
##CXX_FILE
set(PROJECT_SOURCE_FILE ${PROJECT_FILE})
list(FILTER PROJECT_SOURCE_FILE INCLUDE REGEX "\\.h$|\\.cpp$")

set_source_files_properties("MainWidget/ControlWidget/YT.qml" PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_executable(${PROJECT_NAME})
qt_add_qml_module(
    ${PROJECT_NAME} VERSION 1.0
    URI 
        ${PROJECT_NAME}
    QML_FILES 
        "Main.qml" 
        ${PROJECT_QML_FILE}
    SOURCES 
        "main.cpp" 
        ${PROJECT_SOURCE_FILE} 
    DEPENDENCIES 
        QtQuick
)

file(GLOB_RECURSE PROJECT_UI_IMAGE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "YT_Resource/Ui_Image/*")
qt6_add_resources(
    ${PROJECT_NAME} "Ui_Image"
    BASE 
        "YT_Resource"
    FILES
        ${PROJECT_UI_IMAGE} 
)

file(GLOB_RECURSE PROJECT_SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "YT_Resource/Shaders/*")
qt6_add_shaders(
    ${PROJECT_NAME} "Shaders"
    BASE 
        "YT_Resource"
    FILES
        ${PROJECT_SHADERS} 
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE

    # WIN32_EXECUTABLE FALSE 有控制台
    WIN32_EXECUTABLE TRUE
    # WIN32_EXECUTABLE <CONFIG:Release>
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Sql)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Multimedia)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Concurrent)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::QuickControls2)

# set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_QML_IMPORT_PATH "${YT_Material_QML_IMPORT_PATH}")
# target_link_libraries(${PROJECT_NAME} PRIVATE YT_Material::YT_Material)

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_QML_IMPORT_PATH "${CMAKE_CURRENT_BINARY_DIR}/YT_Material")
target_link_libraries(${PROJECT_NAME} PRIVATE YT_Material)

target_link_libraries(${PROJECT_NAME} PRIVATE MediaOperations)

target_link_libraries(${PROJECT_NAME} PRIVATE "${YT_Library}/chinese-util-cpp/lib/libchinese_util.a")
target_include_directories(${PROJECT_NAME} PRIVATE "${YT_Library}/chinese-util-cpp/include")

# target_link_libraries(${PROJECT_NAME} PRIVATE "${YT_Library}/SQLite/sqlite3.dll")
# target_include_directories(${PROJECT_NAME} PRIVATE "${YT_Library}/SQLite")

target_link_libraries(${PROJECT_NAME} PRIVATE HuskarUI::Basic)
# target_link_libraries(${PROJECT_NAME} PRIVATE "${YT_Library}/HuskarUI/bin/HuskarUIBasic.dll")
# target_link_libraries(${PROJECT_NAME} PRIVATE "${YT_Library}/HuskarUI/bin/HuskarUIImpl.dll")
# target_include_directories(${PROJECT_NAME} PRIVATE "${YT_Library}/HuskarUI/include")

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

# # If MediaOperations exported a list of FFmpeg DLLs (Windows), copy them to the exe output after build
# if(WIN32 AND DEFINED MediaOperationsLibrary)
#     foreach(_ff ${MediaOperationsLibrary})
#         if(EXISTS ${_ff})
#             add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                 COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                     "${_ff}"
#                     $<TARGET_FILE_DIR:${PROJECT_NAME}>
#                 COMMENT "Copying FFmpeg DLL ${_ff} to output directory"
#             )
#         endif()
#     endforeach()
# endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT YT_QML_Script
)
install(SCRIPT ${YT_QML_Script})