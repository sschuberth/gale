cmake_minimum_required(VERSION 2.6.4)

# Set some variables and the project name.
set(project_name "gale")
set(header_root "../include")
set(source_root "../source")
set(glex_root "../glex")

project(${project_name})

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
include_directories(${header_root} ${glex_root})

# Specify where to put the compiled library. This is relative to
# CMAKE_CURRENT_BINARY_DIR.
set(LIBRARY_OUTPUT_PATH "../../lib" CACHE INTERNAL "" FORCE)

# Recursively add the source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
file(GLOB_RECURSE headers "${header_root}/*.h" "${header_root}/*.inl")

# Build a sorted list of files, while also specifying the groups they go to.
foreach(header ${headers})
    # Determine the name of the group from the header's parent directory name.
    get_filename_component(path ${header} PATH)
    get_filename_component(group ${path} NAME)

    # Add the header to the list of files.
    list(APPEND files ${header})
    source_group(${group} FILES ${header})

    # Check if there is a source file for this header file, and add it to both
    # the list of files and the group if it exists.
    get_filename_component(name ${header} NAME_WE)
    file(GLOB_RECURSE sources "${source_root}/${name}.cpp" "${source_root}/${name}_*.cpp")
    foreach(source ${sources})
        if(EXISTS ${source})
            list(APPEND files ${source})
            source_group(${group} FILES ${source})
        endif(EXISTS ${source})
    endforeach(source ${sources})
endforeach(header ${headers})

# Add OpenGL extension initialization code to the project.
get_filename_component(group ${glex_root} NAME)

file(GLOB_RECURSE headers "${glex_root}/*.h")
list(APPEND files ${headers})
source_group(${group} FILES ${headers})

file(GLOB_RECURSE sources "${glex_root}/*.c")
list(APPEND files ${sources})
source_group(${group} FILES ${sources})

# Avoid to include the CMakeLists.txt files themselves to the project to get rid
# of the custom build step in Visual Studio that regenerates the project.
set(CMAKE_SUPPRESS_REGENERATION "1")

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
add_library(${project_name} STATIC ${files})

if(NOT WIN32 OR CYGWIN)
    # Get the absolute (!) path to the library we generate here, to pass it to the
    # linker for the test programs. On Linux, this is *not* done automatically by
    # specifying a dependency.
    set(library_name "${LIBRARY_OUTPUT_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${project_name}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    get_filename_component(library_name ${library_name} ABSOLUTE)
endif(NOT WIN32 OR CYGWIN)

# List the libraries all following targets should link against.
include(FindOpenGL)
link_libraries(${OPENGL_gl_LIBRARY} ${library_name})

# Relative to CMAKE_CURRENT_SOURCE_DIR, add more projects to this one by
# refering to their CMakeLists.txt files.
file(GLOB tests "../test/*")
foreach(test ${tests})
    get_filename_component(path "${test}" ABSOLUTE)
    if(IS_DIRECTORY ${path})
        get_filename_component(dir ${path} NAME)
        if(NOT ${dir} STREQUAL ".git" AND NOT ${dir} STREQUAL ".svn")
            add_subdirectory(${test} "${CMAKE_CURRENT_BINARY_DIR}/${dir}")
        endif(NOT ${dir} STREQUAL ".git" AND NOT ${dir} STREQUAL ".svn")
    endif(IS_DIRECTORY ${path})
endforeach(test ${tests})
