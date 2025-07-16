cmake_minimum_required(VERSION 3.29)
project(QtV8)

# 根据自己的环境配置一下三个目录：
# Qt工具套件目录
set(CMAKE_PREFIX_PATH "D:/Qt/6.9.0/msvc2022_64")
# boost目录
set(BOOST_DIR "E:/src/boost_1_85_0")
# v8引擎源码目录
set(V8_SRC_DIR "E:/src/v8_13.0.245.18/v8")

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_definitions(
        -DV8_COMPRESS_POINTERS
        -DV8_ENABLE_SANDBOX
#        -D_ITERATOR_DEBUG_LEVEL=0
        -D_WIN32_WINNT=0x0601
)

if (MSVC)
    add_compile_options(
            /MT
    )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrtd.lib")
endif ()

include_directories(${ROOT_DIR})
include_directories(${BOOST_DIR})
include_directories(${V8_SRC_DIR})
include_directories(${V8_SRC_DIR}/include)
include_directories(${V8_SRC_DIR}/include/libplatform)

link_directories(${V8_SRC_DIR}/out/clang_lib_x64_release/obj)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(QtV8
        ${ROOT_DIR}/src/gui/main.cpp

        ${ROOT_DIR}/src/v8/V8Initializator.cpp
        ${ROOT_DIR}/src/v8/V8Initializator.h
        ${ROOT_DIR}/src/v8/V8Isolate.cpp
        ${ROOT_DIR}/src/v8/V8Isolate.h
        ${ROOT_DIR}/src/v8/V8Context.cpp
        ${ROOT_DIR}/src/v8/V8Context.h

        ${ROOT_DIR}/src/module/wrapper_type_info.cpp
        ${ROOT_DIR}/src/module/wrapper_type_info.h
        ${ROOT_DIR}/src/module/script_wrappable.cpp
        ${ROOT_DIR}/src/module/script_wrappable.h
        ${ROOT_DIR}/src/module/per_isolate_data.cpp
        ${ROOT_DIR}/src/module/per_isolate_data.h

        ${ROOT_DIR}/src/interface/local_dom_window.cpp
        ${ROOT_DIR}/src/interface/local_dom_window.h

        ${ROOT_DIR}/src/gui/QtV8GUI.cpp
        ${ROOT_DIR}/src/gui/QtV8GUI.h
        ${ROOT_DIR}/src/gui/QtV8GUI.ui
        ${ROOT_DIR}/src/gui/QtV8Thread.cpp
        ${ROOT_DIR}/src/gui/QtV8Thread.h
        ${ROOT_DIR}/src/gui/QtV8Runner.cpp
        ${ROOT_DIR}/src/gui/QtV8Runner.h
        ${ROOT_DIR}/src/gui/QtV8ConsoleDelegate.h

        ${ROOT_DIR}/src/gui/interface/Global.cpp
        ${ROOT_DIR}/src/gui/interface/Global.h

        ${ROOT_DIR}/src/gui/inspector/V8InspectorClient.cpp
        ${ROOT_DIR}/src/gui/inspector/V8InspectorClient.h
        ${ROOT_DIR}/src/gui/inspector/V8InspectorChannel.cpp
        ${ROOT_DIR}/src/gui/inspector/V8InspectorChannel.h

        ${ROOT_DIR}/src/gui/inspector/DevToolsSession.cpp
        ${ROOT_DIR}/src/gui/inspector/DevToolsSession.h
        ${ROOT_DIR}/src/gui/inspector/DevToolsAgent.cpp
        ${ROOT_DIR}/src/gui/inspector/DevToolsAgent.h
        ${ROOT_DIR}/src/gui/inspector/Utils.h
)

target_link_libraries(QtV8
        winmm dbghelp
        v8_monolith
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
set_target_properties(QtV8 PROPERTIES WIN32_EXECUTABLE TRUE)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
