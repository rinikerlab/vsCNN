project(CNN)

set(HEADER_FILES
        ../src/neighbors.h
        ../src/geometry.h
        ../src/clustering.h
        ../src/tools/io.h
        )

set(SOURCE_FILES
        ../src/neighbors.cpp
        ../src/geometry.cpp
        ../src/clustering.cpp
        ../src/tools/io.cpp
        )

add_library(lib STATIC ${SOURCE_FILES} ${HEADER_FILES})