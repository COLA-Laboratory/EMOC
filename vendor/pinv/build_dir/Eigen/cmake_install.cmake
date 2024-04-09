# Install script for directory: /home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE FILE FILES
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Cholesky"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/CholmodSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Core"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Dense"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Eigen"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Eigenvalues"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Geometry"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Householder"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/IterativeLinearSolvers"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Jacobi"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/LU"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/MetisSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/OrderingMethods"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/PaStiXSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/PardisoSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/QR"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/QtAlignedMalloc"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SPQRSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SVD"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/Sparse"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SparseCholesky"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SparseCore"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SparseLU"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SparseQR"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/StdDeque"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/StdList"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/StdVector"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/SuperLUSupport"
    "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/UmfPackSupport"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE DIRECTORY FILES "/home/htian/Documents/EMOC-main/vendor/pinv/eigen-3.3.8/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

