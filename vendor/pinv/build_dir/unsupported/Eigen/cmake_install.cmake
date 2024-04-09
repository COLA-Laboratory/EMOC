# Install script for directory: /home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/AdolcForward"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/AlignedVector3"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/ArpackSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/AutoDiff"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/BVH"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/EulerAngles"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/FFT"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/IterativeSolvers"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/KroneckerProduct"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/LevenbergMarquardt"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/MatrixFunctions"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/MoreVectorization"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/MPRealSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/NonLinearOptimization"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/NumericalDiff"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/OpenGLSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/Polynomials"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/Skyline"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/SparseExtra"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/SpecialFunctions"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/Splines"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/htian/Documents/EMOC-main/vendor/pinv/build_dir/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

