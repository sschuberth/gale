# Set the project name to the current directory name prefixed by its parent.
get_filename_component(parent "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(prefix ${parent} NAME)
get_filename_component(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(name "${prefix}_${name}")

project(${name})

# Recursively add the source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
file(GLOB_RECURSE sources "*.h" "*.cpp")

# Append any shader files to the list of source code files.
file(GLOB_RECURSE shaders "*.glsl")
list(APPEND sources ${shaders})

# Add custom build step to convert shader files to header files.
foreach(shader ${shaders})
    get_filename_component(basename ${shader} NAME_WE)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${basename}.inl" COMMAND ../../../res/shader_minifier.exe -o "${CMAKE_CURRENT_SOURCE_DIR}/${basename}.inl" --preserve-externals ${shader} MAIN_DEPENDENCY ${shader})
endforeach()

# Do not create the default "Source Files" group.
source_group("" FILES ${sources})

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
add_executable(${name} ${sources})

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
include_directories("../../include")

# Compile and link against OpenGL and GALE.
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${name} ${OPENGL_LIBRARIES} gale)

# Specify where to put the compiled executable. This is relative to
# CMAKE_CURRENT_BINARY_DIR.
set(EXECUTABLE_OUTPUT_PATH "../../../bin" CACHE INTERNAL "" FORCE)
