cmake_minimum_required(VERSION 3.10)

project(EMOC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE "Release")    			    # make no difference in windows for Visual Studio
set(CMAKE_CONFIGURATION_TYPES "Release;Debug")	# this is for Visual Studio
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Configuration Type: ${CMAKE_CONFIGURATION_TYPES}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Ot")	
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w")
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
endif()	



# set headers
file(GLOB_RECURSE HEADERS ./src/*.h)
set(HEADERS ${HEADERS}
		vendor/random/random.h
		vendor/wfg/iwfg.h
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
)      

# set srcs	
file(GLOB_RECURSE SRC ./src/*.cpp)	
set(SRC ${SRC}
		vendor/random/random.cpp
		vendor/wfg/iwfg.cpp
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
)
		
add_executable(EMOC ${HEADERS} ${SRC})

# add glfw source code
add_subdirectory(vendor/glfw)

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
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${LINK_DIR})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(EMOC glfw3.lib opengl32.lib)
	set_target_properties(EMOC PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SRC})
	add_custom_command(TARGET EMOC POST_BUILD COMMAND copy ..\\build\\Release\\EMOC.exe ..\\EMOC.exe)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(EMOC glfw3 dl X11 GL pthread) 
	add_custom_command(TARGET EMOC POST_BUILD COMMAND mv ./EMOC ../EMOC)
endif()


