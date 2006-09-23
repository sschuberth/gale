PROJECT(gale)

# Recursively add source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
FILE(GLOB_RECURSE sources "*.cpp")

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
ADD_LIBRARY(gale ${sources})

# Specify where to put the compiled library. This is relative to the path where
# the project file is located.
SET(LIBRARY_OUTPUT_PATH "../../lib")

# Specify any required include directories. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project file are absolute.
INCLUDE_DIRECTORIES("../include")

ADD_SUBDIRECTORY("../test/tuple" ${CMAKE_CURRENT_BINARY_DIR})
