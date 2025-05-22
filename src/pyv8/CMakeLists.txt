cmake_minimum_required(VERSION 3.29)

set(PRONAME pyv8)
project(${PRONAME})

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(
            /Zc:__cplusplus
            /utf-8
            /MT
    )
endif ()
add_definitions(-DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set(V8_SRC_DIR "E:/src/v8_13.0.245.18/v8")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".pyd")

include_directories(${ROOT_DIR})
include_directories(${ROOT_DIR}/src)
include_directories(${ROOT_DIR}/src/v8)
#---------------------py-------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pylib/include)
#---------------------v8-------------------------------------
include_directories(${V8_SRC_DIR})
include_directories(${V8_SRC_DIR}/include)
include_directories(${V8_SRC_DIR}/include/libplatform)

#---------------------link-------------------------------------
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/pylib)
link_directories(${V8_SRC_DIR}/out/clang_lib_x64_release/obj)

add_library(${PRONAME} SHARED
    module.cpp
    ${ROOT_DIR}/src/v8/V8Isolate.cpp
    ${ROOT_DIR}/src/v8/V8Isolate.h
    ${ROOT_DIR}/src/v8/V8Context.cpp
    ${ROOT_DIR}/src/v8/V8Context.h
    PyV8Isolate.cpp
    PyV8Isolate.h
    PyV8Context.cpp
    PyV8Context.h
)

target_link_libraries(${PRONAME} winmm dbghelp v8_monolith python313)

set(TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
add_custom_command(TARGET ${PRONAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PRONAME}> ${TARGET_DIR}
)