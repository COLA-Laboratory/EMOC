cmake_minimum_required(VERSION 3.10)

project(EMOC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CONFIGURATION_TYPES "Release")
set(CMAKE_BUILD_TYPE "Debug")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Ot")	
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w")
endif()	

set(INCLUDEDIR 
    ./src
    ./vendor
    ./vendor/imgui
    ./vendor/glfw/include
    # ./vendor/pthread/prebuilt/include
    # for my matplot test
    # F:\Anaconda\include
    # F:\Anaconda\Lib\site-packages\numpy\core\include
)
set(LIBDIR
    ./vendor/glfw/libs
)
	
link_directories(${LIBDIR})
include_directories(${INCLUDEDIR})

set(HEADERS
		vendor/random/random.h
		vendor/wfg/iwfg.h
		vendor/glfw/include/GLFW/glfw3.h
		vendor/imgui/imconfig.h
		vendor/imgui/imgui.h
		vendor/imgui/imgui_internal.h
		vendor/imgui/imstb_rectpack.h
		vendor/imgui/imstb_textedit.h
		vendor/imgui/imstb_truetype.h
        src/core/file.h
        src/core/global.h
        src/core/individual.h
        src/core/nd_sort.h
        src/core/tournament_selection.h
        src/core/uniform_point.h
        src/core/utility.h
        src/core/emoc_manager.h
        src/ui/imgui_impl_glfw.h
        src/ui/imgui_impl_opengl3.h
        src/ui/imgui_impl_opengl3_loader.h
        src/ui/plot.h
        src/ui/uipanel_manager.h
        src/ui/ui_utility.h
        src/ui/test_panel.h
        src/ui/experiment_panel.h
        src/problem/problem.h
        src/problem/problem_head_collect.h
        src/problem/dtlz.h
        src/problem/uf.h
        src/problem/wfg.h
        src/problem/zdt.h
        src/algorithms/algorithm.h
        src/algorithms/algorithm_head_collect.h
        src/algorithms/ens_moead/ens_moead.h
        src/algorithms/hype/hype.h
        src/algorithms/ibea/ibea.h
        src/algorithms/moead/moead.h
        src/algorithms/moead_de/moead_de.h
        src/algorithms/moead_dra/moead_dra.h
        src/algorithms/moead_frrmab/moead_frrmab.h
        src/algorithms/moead_gra/moead_gra.h
        src/algorithms/moead_ira/moead_ira.h
        src/algorithms/nsga2/nsga2.h
        src/algorithms/smsemoa/smsemoa.h
        src/algorithms/spea2/spea2.h
        src/operator/de.h 
        src/operator/mutation.h
        src/operator/sbx.h
        src/metric/gd.h
        src/metric/hv.h
        src/metric/igd.h
        src/metric/spacing.h
        src/emoc_app.h
        )

# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
# 	set(HEADERS ${HEADERS} 
# 		vendor/pthread/prebuilt/include/pthread.h)
# endif()
		
		
set(SRC
		vendor/random/random.cpp
		vendor/wfg/iwfg.cpp
		vendor/imgui/imgui.cpp
		vendor/imgui/imgui_demo.cpp
		vendor/imgui/imgui_draw.cpp
		vendor/imgui/imgui_tables.cpp
		vendor/imgui/imgui_widgets.cpp
        src/core/file.cpp
        src/core/global.cpp
        src/core/individual.cpp
        src/core/nd_sort.cpp
        src/core/tournament_selection.cpp
        src/core/uniform_point.cpp
        src/core/utility.cpp
        src/core/emoc_manager.cpp
        src/ui/imgui_impl_glfw.cpp
        src/ui/imgui_impl_opengl3.cpp
        src/ui/plot.cpp
        src/ui/uipanel_manager.cpp
        src/ui/ui_utility.cpp 
        src/ui/test_panel.cpp
        src/ui/experiment_panel.cpp
        src/problem/problem.cpp
        src/problem/dtlz.cpp
        src/problem/uf.cpp
        src/problem/wfg.cpp
        src/problem/zdt.cpp
        src/algorithms/algorithm.cpp
        src/algorithms/ens_moead/ens_moead.cpp
        src/algorithms/hype/hype.cpp
        src/algorithms/ibea/ibea.cpp
        src/algorithms/moead/moead.cpp
        src/algorithms/moead_de/moead_de.cpp
        src/algorithms/moead_dra/moead_dra.cpp
        src/algorithms/moead_frrmab/moead_frrmab.cpp
        src/algorithms/moead_gra/moead_gra.cpp
        src/algorithms/moead_ira/moead_ira.cpp
        src/algorithms/nsga2/nsga2.cpp
        src/algorithms/smsemoa/smsemoa.cpp
        src/algorithms/spea2/spea2.cpp
        src/operator/de.cpp 
        src/operator/mutation.cpp
        src/operator/sbx.cpp
        src/metric/gd.cpp
        src/metric/hv.cpp
        src/metric/igd.cpp
        src/metric/spacing.cpp
        src/emoc_app.cpp
        src/main_backup.cpp
        src/main.cpp
        )
		
add_executable(EMOC ${HEADERS} ${SRC})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(EMOC glfw3.lib opengl32.lib)
	set_directory_properties(PROPERTIES VS_STARTUP_PROJECT EMOC)
	set_target_properties(EMOC PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SRC})
	# add_custom_command(TARGET EMOC POST_BUILD COMMAND copy ..\\bin\\pthreadVC2.dll ..\\pthreadVC2.dll)
	add_custom_command(TARGET EMOC POST_BUILD COMMAND copy ..\\build\\Release\\EMOC.exe ..\\EMOC.exe)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(EMOC pthread)
	
	add_custom_command(TARGET EMOC POST_BUILD COMMAND mv ./EMOC ../EMOC)
endif()

