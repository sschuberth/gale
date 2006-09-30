PROJECT(gale)

SET(header_root "../include")
SET(source_root "../source")

# Recursively add source code files. The specified path is relative to
# CMAKE_CURRENT_SOURCE_DIR, but the returned paths are absolute.
FILE(GLOB_RECURSE headers "${header_root}/*.h")

# Build a sorted list of files while also specifying the groups they go to.
FOREACH(header ${headers})
    # Add the header to the list of files.
    LIST(APPEND files ${header})

    # Determine the name of the group from the header's parent directory name.
    GET_FILENAME_COMPONENT(path ${header} PATH)
    GET_FILENAME_COMPONENT(group ${path} NAME)

    # Check if there is a source file for this header file, and add it to both
    # the list of files and the group if it exists.
    GET_FILENAME_COMPONENT(name ${header} NAME_WE)
    GET_FILENAME_COMPONENT(source "${source_root}/${name}.cpp" ABSOLUTE)
    IF(EXISTS ${source})
        LIST(APPEND files ${source})
        SOURCE_GROUP(${group} FILES ${header} ${source})
    ELSE(EXISTS ${source})
        SOURCE_GROUP(${group} FILES ${header})
    ENDIF(EXISTS ${source})
ENDFOREACH(header ${headers})

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
ADD_LIBRARY(gale ${files})

# Specify where to put the compiled library. This is relative to
# CMAKE_CURRENT_BINARY_DIR, where the project file is located.
SET(LIBRARY_OUTPUT_PATH CACHE INTERNAL "../../lib")

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
INCLUDE_DIRECTORIES(${header_root})

# List the libraries all following targets should link against.
INCLUDE(FindOpenGL)
LINK_LIBRARIES(${OPENGL_gl_LIBRARY})

# Relative to CMAKE_CURRENT_SOURCE_DIR, add more projects to this one by
# refering to their CMakeLists.txt files.
ADD_SUBDIRECTORY("../test/opengl" ${CMAKE_CURRENT_BINARY_DIR})
ADD_SUBDIRECTORY("../test/tuple" ${CMAKE_CURRENT_BINARY_DIR})
ADD_SUBDIRECTORY("../test/vector" ${CMAKE_CURRENT_BINARY_DIR})
