PROJECT(gale)

# Recursively add source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
FILE(GLOB_RECURSE sources "../source/*.cpp" "../include/*.h")

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
ADD_LIBRARY(gale ${sources})

# Specify where to put the compiled library. This is relative to
# CMAKE_CURRENT_BINARY_DIR, where the project file is located.
SET(LIBRARY_OUTPUT_PATH "../../lib")

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
INCLUDE_DIRECTORIES("../include")

# List the libraries all following targets should link against.
LINK_LIBRARIES(opengl32.lib)

# Relative to CMAKE_CURRENT_SOURCE_DIR, add more projects to this one by
# refering to their CMakeLists.txt files.
ADD_SUBDIRECTORY("../test/opengl" ${CMAKE_CURRENT_BINARY_DIR})
ADD_SUBDIRECTORY("../test/tuple" ${CMAKE_CURRENT_BINARY_DIR})
ADD_SUBDIRECTORY("../test/vector" ${CMAKE_CURRENT_BINARY_DIR})
