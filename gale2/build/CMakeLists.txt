CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(project_name "gale")
SET(header_root "../include")
SET(source_root "../source")
SET(glex_root "../glex")

PROJECT(${project_name})

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
INCLUDE_DIRECTORIES(${header_root} ${glex_root})

# Specify where to put the compiled library. This is relative to
# CMAKE_CURRENT_BINARY_DIR, where the project file is located.
SET(LIBRARY_OUTPUT_PATH "../../lib" CACHE INTERNAL "" FORCE)

# Recursively add source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
FILE(GLOB_RECURSE headers "${header_root}/*.h" "${header_root}/*.inl")

# Build a sorted list of files while also specifying the groups they go to.
FOREACH(header ${headers})
    # Determine the name of the group from the header's parent directory name.
    GET_FILENAME_COMPONENT(path ${header} PATH)
    GET_FILENAME_COMPONENT(group ${path} NAME)

    # Add the header to the list of files.
    LIST(APPEND files ${header})
    SOURCE_GROUP(${group} FILES ${header})

    # Check if there is a source file for this header file, and add it to both
    # the list of files and the group if it exists.
    GET_FILENAME_COMPONENT(name ${header} NAME_WE)
    FILE(GLOB_RECURSE sources "${source_root}/${name}.cpp" "${source_root}/${name}_*.cpp")
    FOREACH(source ${sources})
        IF(EXISTS ${source})
            LIST(APPEND files ${source})
            SOURCE_GROUP(${group} FILES ${source})
        ENDIF(EXISTS ${source})
    ENDFOREACH(source ${sources})
ENDFOREACH(header ${headers})

# Add OpenGL extension initialization code to the project.
GET_FILENAME_COMPONENT(group ${glex_root} NAME)

FILE(GLOB_RECURSE headers "${glex_root}/*.h")
LIST(APPEND files ${headers})
SOURCE_GROUP(${group} FILES ${headers})

FILE(GLOB_RECURSE sources "${glex_root}/*.c")
LIST(APPEND files ${sources})
SOURCE_GROUP(${group} FILES ${sources})

# Avoid to include the CMakeLists.txt files themselves to the project to get rid
# of the custom build step in Visual Studio that regenerates the project.
SET(CMAKE_SUPPRESS_REGENERATION "1")

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
ADD_LIBRARY(${project_name} ${files})

IF(NOT WIN32 OR CYGWIN)
    # Get the absolute (!) path to the library we generate here, to pass it to the
    # linker for the test programs. On Linux, this is *not* done automatically by
    # specifying a dependency.
    SET(library_name ${LIBRARY_OUTPUT_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${project_name}${CMAKE_STATIC_LIBRARY_SUFFIX})
    GET_FILENAME_COMPONENT(library_name ${library_name} ABSOLUTE)
ENDIF(NOT WIN32 OR CYGWIN)

# List the libraries all following targets should link against.
INCLUDE(FindOpenGL)
LINK_LIBRARIES(${OPENGL_gl_LIBRARY} ${library_name})

# Relative to CMAKE_CURRENT_SOURCE_DIR, add more projects to this one by
# refering to their CMakeLists.txt files.
FILE(GLOB tests "../test/*")
FOREACH(test ${tests})
    GET_FILENAME_COMPONENT(path "${test}" ABSOLUTE)
    IF(IS_DIRECTORY ${path})
        GET_FILENAME_COMPONENT(dir ${path} NAME)
        IF(NOT ${dir} STREQUAL "CVS" AND NOT ${dir} STREQUAL ".svn")
            ADD_SUBDIRECTORY(${test} ${CMAKE_CURRENT_BINARY_DIR})
        ENDIF(NOT ${dir} STREQUAL "CVS" AND NOT ${dir} STREQUAL ".svn")
    ENDIF(IS_DIRECTORY ${path})
ENDFOREACH(test ${tests})
