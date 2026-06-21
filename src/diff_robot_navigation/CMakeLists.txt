cmake_minimum_required(VERSION 3.8)
project(diff_robot_navigation)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

include_directories(include)

add_library(obstacle_avoidance_lib SHARED
  src/obstacle_avoidance.cpp
)
ament_target_dependencies(obstacle_avoidance_lib
  rclcpp
  sensor_msgs
  geometry_msgs
)

add_executable(obstacle_avoidance
  src/obstacle_avoidance_main.cpp
)
target_link_libraries(obstacle_avoidance
  obstacle_avoidance_lib
)
ament_target_dependencies(obstacle_avoidance
  rclcpp
)

install(TARGETS
  obstacle_avoidance_lib
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS
  obstacle_avoidance
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

ament_export_include_directories(include)
ament_export_libraries(obstacle_avoidance_lib)
ament_export_dependencies(
  rclcpp
  sensor_msgs
  geometry_msgs
)

ament_package()