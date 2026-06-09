# 差速驱动机器人项目 (Diff Robot)

基于ROS2的差速驱动机器人仿真与控制系统。

## 环境要求

- **操作系统**: Ubuntu 22.04 LTS (推荐)
- **ROS2版本**: Humble Hawksbill
- **依赖**:
  - ros-humble-desktop
  - ros-humble-ros2-control
  - ros-humble-ros2-controllers
  - ros-humble-gazebo-ros-pkgs
  - ros-humble-xacro

## 快速开始

### 1. 环境配置

```bash
source scripts/env_setup.sh
```

### 2. 一键编译

```bash
./scripts/build_all.sh
```

### 3. 启动仿真

```bash
ros2 launch diff_robot_control gazebo.launch.py
```

### 4. 键盘遥控

```bash
ros2 run diff_robot_teleop teleop_twist_keyboard
```

## 安装依赖

```bash
# 安装ROS2 Humble (如未安装)
sudo apt install ros-humble-desktop

# 安装额外依赖
sudo apt install \
  ros-humble-ros2-control \
  ros-humble-ros2-controllers \
  ros-humble-gazebo-ros-pkgs \
  ros-humble-xacro \
  ros-humble-joint-state-publisher-gui
```

## 编译

```bash
# 进入工作空间
cd diff_robot

# 安装依赖
rosdep install --from-paths src --ignore-src -r -y

# 编译
colcon build --symlink-install

# 加载环境
source install/setup.bash
```

## 启动

### 1. 启动Gazebo仿真

```bash
ros2 launch diff_robot_control gazebo.launch.py
```

### 2. 启动键盘遥控

```bash
ros2 run diff_robot_teleop teleop_twist_keyboard
```

### 3. 查看机器人模型

```bash
ros2 launch diff_robot_description view_robot.launch.py
```

## 使用说明

### 话题列表

| 话题名称 | 类型 | 说明 |
|---------|------|------|
| `/cmd_vel` | geometry_msgs/Twist | 速度控制指令 |
| `/odom` | nav_msgs/Odometry | 里程计数据 |
| `/joint_states` | sensor_msgs/JointState | 关节状态 |

### 速度控制示例

```bash
# 发布前进指令
ros2 topic pub /cmd_vel geometry_msgs/Twist "{linear: {x: 0.5}, angular: {z: 0.0}}"
```

## 功能包说明

### diff_robot_description

机器人描述文件包，包含：
- URDF/xacro 机器人模型
- RViz 配置文件

### diff_robot_control

仿真与控制包，包含：
- Gazebo 仿真世界和启动文件
- ros2_control 控制器配置
- 差速驱动控制器

### diff_robot_teleop

遥控节点包，包含：
- 键盘遥控节点
- 支持速度动态调节

## 许可证

Apache License 2.0
