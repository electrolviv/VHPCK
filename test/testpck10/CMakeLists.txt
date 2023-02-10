cmake_minimum_required(VERSION 3.1)

set(PRJNAME testpackv10)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(${PRJNAME})


set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -ggdb")

set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-save-temps -fverbose-asm")
#target_compile_options(${TARGET} PRIVATE "-save-temps -fverbose-asm")

file(GLOB_RECURSE srclist_src      src/*.cpp src/*.hpp)
file(GLOB_RECURSE srclist_pck      ../../pck10/*.cpp ../../pck10/*.hpp ../../pck10/*.h)

add_compile_definitions(VHPCK_USING_ENCODER)

add_executable(${PRJNAME}
    ${srclist_src}
    ${srclist_pck}
)

target_include_directories(${PRJNAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../pck10>
)
