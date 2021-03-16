project(CNN)

set(HEADER_FILES
        ../src/neighbors.h
        ../src/geometry.h
        ../src/clustering.h
        ../src/core.h
        ../src/cnn.h
        ../src/vs_cnn.h
        ../src/tools/io.h
        ../src/tools/utility.h
        )

set(SOURCE_FILES
        ../src/neighbors.cpp
        ../src/geometry.cpp
        ../src/clustering.cpp
        ../src/core.cpp
        ../src/cnn.cpp
        ../src/vs_cnn.cpp
        ../src/tools/io.cpp
        ../src/tools/utility.cpp
        )

add_library(lib STATIC ${SOURCE_FILES} ${HEADER_FILES})