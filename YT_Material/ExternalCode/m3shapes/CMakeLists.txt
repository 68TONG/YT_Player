cmake_minimum_required(VERSION 3.30)
project(M3Shapes VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml ShaderTools)

qt_add_library(m3shapes STATIC)

target_sources(m3shapes PRIVATE
    include/m3shapes/core/point.hpp
    include/m3shapes/core/utils.hpp
    include/m3shapes/core/cornerrounding.hpp
    include/m3shapes/core/cubic.hpp
    src/core/cubic.cpp
    include/m3shapes/core/feature.hpp
    src/core/feature.cpp
    include/m3shapes/core/roundedpolygon.hpp
    src/core/roundedpolygon.cpp
    include/m3shapes/morph/floatmapping.hpp
    src/morph/floatmapping.cpp
    include/m3shapes/morph/polygonmeasure.hpp
    src/morph/polygonmeasure.cpp
    include/m3shapes/morph/featuremapping.hpp
    src/morph/featuremapping.cpp
    include/m3shapes/morph/morph.hpp
    src/morph/morph.cpp
    include/m3shapes/shapes/shapes.hpp
    src/shapes/shapes.cpp
    include/m3shapes/shapes/materialshapes.hpp
    src/shapes/materialshapes.cpp
    include/m3shapes/qml/materialshapeitem.hpp
    src/qml/materialshapeitem.cpp
    include/m3shapes/qml/smoothshapematerial.hpp
    src/qml/smoothshapematerial.cpp
)

qt_add_shaders(m3shapes "m3shapes_shaders"
    PREFIX "/m3shapes"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    FILES
        src/shaders/smoothshape.vert
        src/shaders/smoothshape.frag
)

target_compile_features(m3shapes PUBLIC cxx_std_20)
set_target_properties(m3shapes PROPERTIES
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(m3shapes PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(m3shapes PUBLIC
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
)
