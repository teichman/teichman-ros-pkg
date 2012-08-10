cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)



rosbuild_add_library(${PROJECT_NAME}
  src/random.cpp
  )

rosbuild_add_boost_directories()
rosbuild_link_boost(${PROJECT_NAME} system)

rosbuild_add_gtest(test_random src/test_random.cpp)
target_link_libraries(test_random ${PROJECT_NAME})

rosbuild_add_gtest(test_eigen_extensions src/test_eigen_extensions.cpp)
rosbuild_link_boost(test_eigen_extensions filesystem system)

rosbuild_add_executable(cat src/cat.cpp)
rosbuild_link_boost(cat filesystem system)

rosbuild_add_executable(convert src/convert.cpp)
rosbuild_link_boost(convert filesystem system)