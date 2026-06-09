#!/bin/bash
# 使用方法: ./scripts/build_all.sh [--clean] [--verbose]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WS_DIR="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "  差速驱动机器人编译"
echo "=========================================="

cd "$WS_DIR"

if [ -z "$ROS_DISTRO" ]; then
    echo "[INFO] 加载ROS2 Humble环境..."
    source /opt/ros/humble/setup.bash
fi

echo "[INFO] ROS2版本: $ROS_DISTRO"
echo "[INFO] 工作空间: $WS_DIR"

CLEAN=false
VERBOSE=""
for arg in "$@"; do
    case $arg in
        --clean)
            CLEAN=true
            shift
            ;;
        --verbose)
            VERBOSE="--cmake-args -DCMAKE_VERBOSE_MAKEFILE=ON"
            shift
            ;;
    esac
done

if [ "$CLEAN" = true ]; then
    echo "[INFO] 清理编译产物..."
    rm -rf build/ install/ log/
fi

echo "[INFO] 检查并安装依赖..."
rosdep install --from-paths src --ignore-src -r -y 2>/dev/null || true

echo "[INFO] 开始编译..."
colcon build --symlink-install $VERBOSE

echo ""
echo "=========================================="
echo "  编译完成！"
echo "  请执行: source install/setup.bash"
echo "=========================================="
