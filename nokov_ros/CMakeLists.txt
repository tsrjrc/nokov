cmake_minimum_required(VERSION 2.8.3)
project(nokov_ros)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
	INCLUDE_DIRS include
	CATKIN_DEPENDS roscpp rospy std_msgs
)

###########
## Build ##
###########
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)


add_library(${PROJECT_NAME} src/m3x3.cpp src/cortex_socket.cpp src/cortex_unpack.cpp src/cortex.cpp)
target_link_libraries(${PROJECT_NAME} pthread m)

add_executable(${PROJECT_NAME}_node src/clienttest.cpp)
target_link_libraries(${PROJECT_NAME}_node  ${PROJECT_NAME} ${catkin_LIBRARIES})


