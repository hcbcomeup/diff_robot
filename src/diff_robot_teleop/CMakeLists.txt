cmake_minimum_required(VERSION 3.8)
project(diff_robot_teleop)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

include_directories(
  include
)

add_executable(teleop_twist_keyboard
  src/teleop_twist_keyboard.cpp
  src/teleop_twist_keyboard_main.cpp
)
ament_target_dependencies(teleop_twist_keyboard
  rclcpp
  geometry_msgs
)

install(TARGETS
  teleop_twist_keyboard
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()