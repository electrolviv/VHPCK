cmake_minimum_required(VERSION 3.1)

set(PRJNAME testunpack)

project(${PRJNAME})


set(CMAKE_CXX_STANDARD 14)


file(GLOB_RECURSE srclist_src      src/*.cpp src/*.hpp )
file(GLOB_RECURSE srclist_pck      ../../pck/*.cpp ../../pck/*.hpp ../../pck/*.h )


add_executable(${PRJNAME}
    ${srclist_src} ${srclist_pck}
)

target_include_directories(${PRJNAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../pck>

)

