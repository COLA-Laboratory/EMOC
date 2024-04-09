cmake_minimum_required(VERSION 3.10)

project(EMOC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "set build type to release")    			    # make no difference in windows for Visual Studio
set(CMAKE_CONFIGURATION_TYPES "Release")	# this is for Visual Studio
set(BUILD_PYTHON_DLL OFF CACHE BOOL "Whether build python dll")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Configuration Type: ${CMAKE_CONFIGURATION_TYPES}")

# set compiler optimization flags
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Ot")	
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w")
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w")
    find_library(COCOA_LIBRARY Cocoa)
    if(NOT COCOA_LIBRARY)
        message(FATAL_ERROR "COCOA LIBRARY NOT FOUND")
    endif()
    find_library(IOKIT_LIBRARY IOKit)
    if(NOT IOKIT_LIBRARY)
        message(FATAL_ERROR "IOKIT LIBRARY NOT FOUND")
    endif()
    find_package(OpenGL REQUIRED)
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} Is Not Support  Now!")
endif()	



# search files
file(GLOB_RECURSE SRC ./src/*.cpp)
file(GLOB_RECURSE HEADERS ./src/*.h)
file(GLOB_RECURSE MAIN_NAME ./src/main.cpp)
file(GLOB_RECURSE PYTHON_MAIN_NAME ./src/python_main.cpp)

# set HEADERS
set(HEADERS ${HEADERS}
		vendor/random/random.h
		vendor/glfw/include/GLFW/glfw3.h
		vendor/imgui/imconfig.h
		vendor/imgui/imgui.h
		vendor/imgui/imgui_internal.h
		vendor/imgui/imstb_rectpack.h
		vendor/imgui/imstb_textedit.h
		vendor/imgui/imstb_truetype.h
        vendor/IconsFontAwesome5/IconsFontAwesome5.h
        vendor/alglib/src/alglibinternal.h
        vendor/alglib/src/alglibmisc.h
        vendor/alglib/src/ap.h
        vendor/alglib/src/linalg.h
        vendor/alglib/src/specialfunctions.h
        vendor/alglib/src/statistics.h
        vendor/alglib/src/stdafx.h
        vendor/cxxopts/cxxopts.hpp
        vendor/stb_image/stb_image.h
        vendor/pinv/calculatePinv.h

)      

# set SRC		
set(SRC ${SRC}
		vendor/random/random.cpp
		vendor/imgui/imgui.cpp
		vendor/imgui/imgui_demo.cpp
		vendor/imgui/imgui_draw.cpp
		vendor/imgui/imgui_tables.cpp
		vendor/imgui/imgui_widgets.cpp
        vendor/alglib/src/alglibinternal.cpp
        vendor/alglib/src/alglibmisc.cpp
        vendor/alglib/src/ap.cpp
        vendor/alglib/src/linalg.cpp
        vendor/alglib/src/specialfunctions.cpp
        vendor/alglib/src/statistics.cpp  
        vendor/pinv/calculatePinv.cpp
)

# set include dir and link dir
set(INCLUDE_DIR 
    ./src
    ./vendor
    ./vendor/imgui
    ./vendor/glfw/include
    ./vendor/IconsFontAwesome5
)

set(LINK_DIR 
    ./build/vendor/glfw/src/Debug
    ./build/vendor/glfw/src/Release
    ./build/vendor/glfw/src
	./cmake-build-debug/vendor/glfw/src
	./cmake-build-release/vendor/glfw/src
)

# add glfw source code as a subproject
add_subdirectory(vendor/glfw)	

# create real EMOC project
if(BUILD_PYTHON_DLL)
	list(REMOVE_ITEM SRC ${MAIN_NAME})
	add_subdirectory(vendor/pybind11)
	pybind11_add_module(EMOC ${HEADERS} ${SRC})
else()
	list(REMOVE_ITEM SRC ${PYTHON_MAIN_NAME})
	add_executable(EMOC ${HEADERS} ${SRC})
endif()

# add include dir and link dir
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${LINK_DIR})


# link library and some post build commang
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(EMOC PRIVATE glfw3.lib opengl32.lib)
	set_target_properties(EMOC PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SRC})
	if(NOT BUILD_PYTHON_DLL)
		add_custom_command(TARGET EMOC POST_BUILD COMMAND copy ..\\build\\Release\\EMOC.exe ..\\EMOC.exe)
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(EMOC PRIVATE glfw3 dl X11 GL pthread) 
	if(NOT BUILD_PYTHON_DLL)
		add_custom_command(TARGET EMOC POST_BUILD COMMAND mv ./EMOC ../EMOC)
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # target_link_libraries(EMOC PRIVATE glfw3 ${OPENGL_LIBRARY} ${COCOA_LIBRARY}  ${IOKIT_LIBRARY}) 
    target_link_libraries(EMOC PRIVATE glfw ${OPENGL_LIBRARY} ${COCOA_LIBRARY}  ${IOKIT_LIBRARY}) 
    if(NOT BUILD_PYTHON_DLL)
        # add_custom_command(TARGET EMOC POST_BUILD COMMAND mv ./EMOC ../EMOC)
    endif()
endif()



