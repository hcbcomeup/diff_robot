#!/bin/bash
source /opt/ros/humble/setup.bash
source /ws_robot/install/setup.bash

echo "环境就绪 | ROS: ${ROS_DISTRO} | 仿真: ros2 launch diff_robot_control gazebo.launch.py"

exec "$@"