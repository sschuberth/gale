GET_FILENAME_COMPONENT(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
SET(name test_${name})

PROJECT(${name})

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
INCLUDE_DIRECTORIES("../../include")

# Also specify any directories in which to search for libraries.
LINK_DIRECTORIES("../../lib")

# Specify where to put the compiled executable. This is relative to
# CMAKE_CURRENT_BINARY_DIR, where the project file is located.
SET(EXECUTABLE_OUTPUT_PATH CACHE INTERNAL "../../bin")

# Recursively add source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
FILE(GLOB_RECURSE sources "*.h" "*.cpp")

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
ADD_EXECUTABLE(${name} ${sources})

# Make this test executable project depend on the gale library project.
ADD_DEPENDENCIES(${name} gale)
