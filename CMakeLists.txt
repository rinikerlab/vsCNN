cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 14)

# added -fopenmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -D_GLIBCXX_PARALLEL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec-optimized=vec.info")

### LIB ###
include_directories(lib)
add_subdirectory(lib)
target_link_libraries(lib)

### BINARY EXECUTABLE ###
project(comdensity)

add_executable(comdensity src/main.cpp
        src/datatypes.h
        src/tools/ArgParse.cpp src/tools/ArgParse.h
        src/tools/Argument.cpp src/tools/Argument.h
        src/tools/io.cpp src/tools/io.h
        src/neighbors.cpp src/neighbors.h
        src/geometry.cpp src/geometry.h
        src/core.cpp src/core.h
        src/clustering.cpp src/clustering.h
        src/cnn.h src/cnn.cpp
        src/vs_cnn.h src/vs_cnn.cpp
        src/discretization.h src/discretization.cpp
        src/wrapper.h src/wrapper.cpp
        src/tools/utility.h src/tools/utility.cpp
        )

### TESTS ###
add_subdirectory(tests)

### PYTHON LIBRARY ###

project(pycomdensity)

file(GLOB_RECURSE PYTHONBIND_CPPS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(PYBIND11_USE_CMAKE True)

##get pybind project
add_subdirectory(lib/external/pybind11)
##add pybind module
pybind11_add_module(pycomdensity ${PYTHONBIND_CPPS})
##link_libs
#target_link_libraries(pycomdensity PRIVATE lib)
##install
install(TARGETS pycomdensity LIBRARY DESTINATION lib)
