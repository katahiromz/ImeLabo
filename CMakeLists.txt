# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.6)

# project name and languages
project(ImeLabo VERSION 0.1 LANGUAGES CXX RC)

# CMake build type
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# statically linking
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (MSVC)
    # replace "/MD" with "/MT" (building without runtime DLLs)
    set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    foreach(CompilerFlags ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
    endforeach()
endif()

# nlohmann/json include
include_directories(json/include)

# GOOGLE_INSIDE option
option(GOOGLE_INSIDE "Use Google for server" OFF)
if(GOOGLE_INSIDE)
    add_definitions(-DGOOGLE_INSIDE)
endif()
message(STATUS "GOOGLE_INSIDE = ${GOOGLE_INSIDE}")

# CONSOLE_IS_UTF8 option
option(CONSOLE_IS_UTF8 "Console is UTF-8 output" OFF)
if(CONSOLE_IS_UTF8)
    add_definitions(-DCONSOLE_IS_UTF8)
endif()
message(STATUS "CONSOLE_IS_UTF8 = ${CONSOLE_IS_UTF8}")

##############################################################################

# server.exe
add_executable(server server.cpp util.cpp convert.cpp client.cpp)
target_link_libraries(server ws2_32 urlmon)

# convert_test.exe
add_executable(convert_test convert_test.cpp util.cpp client.cpp convert.cpp)
target_link_libraries(convert_test urlmon)

# net0_test.exe
add_executable(net0_test net0_test.cpp net0_test.cpp util.cpp client.cpp convert.cpp)
target_link_libraries(net0_test urlmon)

# net1_test.exe
add_executable(net1_test net1_test.cpp net1_test.cpp util.cpp client.cpp convert.cpp)
target_link_libraries(net1_test urlmon)

# Support UTF-8 source
if(MSVC)
    target_compile_options(server PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
    target_compile_options(convert_test PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
    target_compile_options(net0_test PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
    target_compile_options(net1_test PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

##############################################################################
