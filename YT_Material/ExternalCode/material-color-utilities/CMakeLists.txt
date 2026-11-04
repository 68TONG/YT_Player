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

file(GLOB_RECURSE MATERIAL_COLOR_UTILITIES_SOURCE_FILE CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cc"
)

add_library(material-color-utilities STATIC ${MATERIAL_COLOR_UTILITIES_SOURCE_FILE})
target_include_directories(material-color-utilities PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(material-color-utilities PRIVATE
    absl::strings
    absl::flat_hash_map
)