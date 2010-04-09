# Set the project name to the current directory name prefixed by its parent.
get_filename_component(parent "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(prefix ${parent} NAME)
get_filename_component(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(name "${prefix}_${name}")

project(${name})

# Recursively add the source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
file(GLOB_RECURSE sources "*.h" "*.cpp")

# Do not create the default "Source Files" group.
source_group("" FILES ${sources})

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
add_executable(${name} ${sources})

# Specify include directories for the compiler. Relative paths are converted to
# absolute paths by appending them to CMAKE_CURRENT_SOURCE_DIR before writing
# them to the project file.
include_directories("../../include")

# Specify library directories for the linker. Relative paths are converted to
# absolute paths by appending them to CMAKE_CURRENT_SOURCE_DIR before writing
# them to the project file. (Policy CMP0015)
link_directories("../../lib")

# Compile and link against OpenGL and GALE.
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

target_link_libraries(${name} ${OPENGL_LIBRARIES})
if(MSVC)
    # Crinkler gets confused by absolute paths to libraries and by having GALE
    # as both an implicit dependency and an explicit link library. So just use
    # an explicit dependency instead.
    add_dependencies(${name} gale)
else()
    target_link_libraries(${name} gale)
endif()

# Specify where to put the compiled executable. This is relative to
# CMAKE_CURRENT_BINARY_DIR.
set(EXECUTABLE_OUTPUT_PATH "../../../bin" CACHE INTERNAL "" FORCE)
