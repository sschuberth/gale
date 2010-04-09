# See http://www.cmake.org/Wiki/CMake_Useful_Variables for some general help.
cmake_minimum_required(VERSION 2.6.4)

# Set some variables and the project name.
set(project_name "gale")
set(header_root "../include")
set(source_root "../source")
set(glex_root "../glex")

project(${project_name})

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
        endif()
    endforeach()
endforeach()

# Add OpenGL extension initialization code to the project.
get_filename_component(group ${glex_root} NAME)

file(GLOB_RECURSE headers "${glex_root}/*.h")
list(APPEND files ${headers})
source_group(${group} FILES ${headers})

file(GLOB_RECURSE sources "${glex_root}/*.c")
list(APPEND files ${sources})
source_group(${group} FILES ${sources})

# Set some custom compiler flags for the heavily size optimized build.
if(MSVC)
    # Flags specified later will override earlier flags.

    # Compiler flags that are properly reflected in the IDE:
    # - Preprocessor Definitions
    # - Enable Intrinsic Functions
    # - Favor Small Code
    # - Omit Frame Pointers
    # - Multi-threaded Runtime Library
    # - Disable Buffer Security Check
    # - Enable Function-Level Linking
    # - Disable Run-Time Type Info
    # - Calling Convention "__fastcall"
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}
        /D GALE_TINY_CODE /D GALE_USE_SSE3
        /Oi
        /Os
        /Oy
        /MT
        /GS-
        /Gy
        /GR-
        /Gr
    ")

    # Compiler flags that are not properly reflected in the IDE:
    # - Enable String Pooling
    # - Disable C++ Exceptions
    # - Floating Point Model
    # - "suppress _ftol" (NOTE: Makes casts use the current FPU rounding mode)
    # - "omit default library name in .OBJ"
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}
        /GF
        /EHs-c-
        /fp:fast
        /Gs999999
        /QIfist
        /Zl
    ")

    # Linker flags that are properly reflected in the IDE:
    # - Ignore All Default Libraries
    # - Do not Generate Manifest
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}
        /NODEFAULTLIB
        /MANIFEST:NO
    ")

    # Linker flags that are not properly reflected in the IDE:
    # - Eliminate Unreferenced Data
    # - Do not Optimize for Windows98
    # - Disable Image Randomization
    # - Image must be loaded at a fixed address
    # - "Merge read-only data and code sections"
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}
        /OPT:REF
        /OPT:NOWIN98
        /DYNAMICBASE:NO
        /FIXED
        /MERGE:.rdata=.text
    ")
endif()

# Avoid to include the CMakeLists.txt files themselves to the project to get rid
# of the custom build step in Visual Studio that regenerates the project.
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# Use relative instead of absolute paths in generated project files.
set(CMAKE_USE_RELATIVE_PATHS TRUE)

# Specify where to put the compiled library. This is relative to
# CMAKE_CURRENT_BINARY_DIR.
set(LIBRARY_OUTPUT_PATH "../../lib" CACHE INTERNAL "" FORCE)

# This writes absolute paths to the generated files even if relative paths are
# specified, which is why we do not care to do a RELATIVE GLOB_RECURSE above.
add_library(${project_name} STATIC ${files})

# Specify any required include directories. The specified path is interpreted as
# relative to CMAKE_CURRENT_SOURCE_DIR, but the paths written to the project
# file are absolute.
include_directories(${header_root} ${glex_root})

# Relative to CMAKE_CURRENT_SOURCE_DIR, add the demo and test projects to this
# one by refering to their CMakeLists.txt files.
file(GLOB demos "../demo/*")
foreach(demo ${demos})
    get_filename_component(path "${demo}" ABSOLUTE)
    if(IS_DIRECTORY ${path})
        get_filename_component(dir ${path} NAME)
        if(NOT ${dir} STREQUAL ".git" AND NOT ${dir} STREQUAL ".svn")
            add_subdirectory(${demo} "${CMAKE_CURRENT_BINARY_DIR}/${dir}")
        endif()
    endif()
endforeach()

file(GLOB tests "../test/*")
foreach(test ${tests})
    get_filename_component(path "${test}" ABSOLUTE)
    if(IS_DIRECTORY ${path})
        get_filename_component(dir ${path} NAME)
        if(NOT ${dir} STREQUAL ".git" AND NOT ${dir} STREQUAL ".svn")
            add_subdirectory(${test} "${CMAKE_CURRENT_BINARY_DIR}/${dir}")
        endif()
    endif()
endforeach()
