project(RUSTLING)
cmake_minimum_required(VERSION 2.8)

set (lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${lib_dir})

#RUSTLING library
add_library(RUSTLING SHARED ${RUSTLING_HEADERS} ${RUSTLING_SOURCE})
include_directories(${RUSTLING_INC_PATH})

#RUSTLING tests executable
add_executable(RUSTLING_APP ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

#Find OpenCL
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(OpenCL REQUIRED)
if(OPENCL_FOUND)
  include_directories(${OPENCL_INCLUDE_DIRS})
  target_link_libraries(RUSTLING ${OPENCL_LIBRARIES})
endif(OPENCL_FOUND)

#Link tests with RUSTLING library
target_link_libraries(RUSTLING_APP RUSTLING)

#Set compiler flags
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_BUILD_TYPE Debug)
