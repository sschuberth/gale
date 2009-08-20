# Set the project name to the current directory name prefixed by its parent.
get_filename_component(parent "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(prefix ${parent} NAME)
get_filename_component(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(name "${prefix}_${name}")

project(${name})

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
include_directories("../../include")

# Also specify any directories in which to search for libraries to link against.
# Relative paths are passed unchanged to the linker.
link_directories("../../lib")

# Specify where to put the compiled executable. This is relative to
# CMAKE_CURRENT_BINARY_DIR.
set(EXECUTABLE_OUTPUT_PATH "../../../bin" CACHE INTERNAL "" FORCE)

# Recursively add the source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
file(GLOB_RECURSE sources "*.h" "*.cpp")

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
add_executable(${name} ${sources})

# Make this test executable project depend on the gale library project.
target_link_libraries(${name} gale ${OPENGL_gl_LIBRARY})
