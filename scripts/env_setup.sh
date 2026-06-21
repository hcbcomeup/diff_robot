#!/bin/bash
# env_setup.sh - 环境配置脚本

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WS_DIR="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "  差速驱动机器人环境变量配置"
echo "=========================================="

if [ -z "$ROS_DISTRO" ]; then
    if [ -f /opt/ros/humble/setup.bash ]; then
        echo "[INFO] 加载ROS2 Humble..."
        source /opt/ros/humble/setup.bash
    else
        echo "[ERROR] 未找到ROS2 Humble，请先安装"
        exit 1
    fi
fi

echo "[INFO] ROS2版本: $ROS_DISTRO"

if [ -f "$WS_DIR/install/setup.bash" ]; then
    echo "[INFO] 加载工作空间..."
    source "$WS_DIR/install/setup.bash"
else
    echo "[WARN] 工作空间尚未编译，请先执行: ./scripts/build_all.sh"
fi

if [ -d "$WS_DIR/src/diff_robot_description/meshes" ]; then
    export GAZEBO_MODEL_PATH="$WS_DIR/src/diff_robot_description/meshes:${GAZEBO_MODEL_PATH:-}"
    echo "[INFO] Gazebo模型路径已配置"
fi

export ROS_DOMAIN_ID=${ROS_DOMAIN_ID:-0}
echo "[INFO] ROS_DOMAIN_ID: $ROS_DOMAIN_ID"

echo ""
echo "=========================================="
echo "  环境配置完成！"
echo "=========================================="
echo ""
echo "支持以下命令:"
echo "  启动仿真:  ros2 launch diff_robot_control gazebo.launch.py"
echo "  键盘遥控:  ros2 run diff_robot_teleop teleop_twist_keyboard"
echo "  查看模型:  ros2 launch diff_robot_description view_robot.launch.py"
echo ""