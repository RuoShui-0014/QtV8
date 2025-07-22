cmake_minimum_required(VERSION 3.29)
project(QtV8)

# 根据自己的环境配置一下三个目录：
# Qt工具套件目录
set(CMAKE_PREFIX_PATH "D:/Qt/6.9.1/msvc2022_64_static")
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
	-D_ITERATOR_DEBUG_LEVEL=0
	-D_WIN32_WINNT=0x0601
)

if (MSVC)
	add_compile_options(
		/MT /EHsc
		-Wno-unused-result
		-Wno-deprecated-literal-operator
	)
else ()
	# 通用GCC/Clang/Intel编译器选项
	add_compile_options(
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Intel>>:
		-Wall
		-Wextra
		-Wno-unused-result
		-Wno-deprecated-literal-operator
	)

	# 静态链接选项（适用于GCC/Clang）
	if (BUILD_STATIC)
		add_compile_options(-static)
		add_link_options(-static)

		# Windows MinGW需要额外系统库
		if (MINGW)
			add_link_options(
				-static-libgcc
				-static-libstdc++
				-Wl,--whole-archive -lwinpthread -Wl,--no-whole-archive
			)
		endif ()
	endif ()
endif ()

# 苹果平台特殊处理
if (APPLE)
	add_compile_options(-stdlib=libc++)
	add_link_options(-stdlib=libc++)
endif ()

# Linux平台特殊处理
if (LINUX)
	add_compile_options(-fPIC)  # 位置无关代码
endif ()

include_directories(${ROOT_DIR})
include_directories(${BOOST_DIR})
include_directories(${V8_SRC_DIR})
include_directories(${V8_SRC_DIR}/include)
include_directories(${V8_SRC_DIR}/include/libplatform)

link_directories(${V8_SRC_DIR}/out/clang_lib_x64_release/obj)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

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
