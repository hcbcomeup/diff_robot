# 差速驱动机器人项目 (Diff Robot)

基于ROS2的差速驱动机器人仿真与控制系统，支持Gazebo物理仿真、键盘遥控及基于势场法（APF）的自动避障导航。

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

### 5. 自动避障导航

```bash
ros2 launch diff_robot_navigation navigation.launch.py
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

### 4. 启动自动避障

```bash
ros2 launch diff_robot_navigation navigation.launch.py
```

### 5. Docker 运行

```bash
# 构建并启动
docker compose -f docker/docker-compose.yaml build
docker compose -f docker/docker-compose.yaml up -d
docker compose -f docker/docker-compose.yaml exec diff-robot bash

# 容器内运行
ros2 launch diff_robot_control gazebo.launch.py
```

## 使用说明

### 话题列表

| 话题名称 | 类型 | 说明 |
|---------|------|------|
| `/diff_drive_controller/cmd_vel_unstamped` | geometry_msgs/Twist | 速度控制指令（差速驱动控制器） |
| `/odom` | nav_msgs/Odometry | 里程计数据 |
| `/joint_states` | sensor_msgs/JointState | 关节状态 |
| `/scan` | sensor_msgs/LaserScan | 激光雷达扫描数据（避障输入） |

### 键盘遥控说明

| 按键 | 功能 |
|------|------|
| `w` | 前进 |
| `x` | 后退 |
| `a` | 左转 |
| `d` | 右转 |
| `s` | 停止 |
| `q/z` | 增加/减少线速度 (10%) |
| `e/c` | 增加/减少角速度 (10%) |

遥控节点可配置参数（通过 `--ros-args -p` 或 YAML 配置）：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `linear_speed` | 0.5 | 初始线速度 (m/s) |
| `angular_speed` | 1.0 | 初始角速度 (rad/s) |
| `max_linear_speed` | 2.0 | 最大线速度 (m/s) |
| `max_angular_speed` | 4.0 | 最大角速度 (rad/s) |
| `cmd_vel_topic` | `/diff_drive_controller/cmd_vel_unstamped` | 发布的速度话题 |

### 避障节点参数

避障节点使用人工势场法（APF），参数可通过 [`obstacle_avoidance.yaml`](src/diff_robot_navigation/config/obstacle_avoidance.yaml) 配置：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `linear_speed` | 0.3 | 最大线速度 (m/s) |
| `angular_speed` | 0.8 | 最大角速度 (rad/s) |
| `safety_distance` | 0.3 | 安全距离（小于此距离紧急停止）(m) |
| `influence_distance` | 1.5 | 障碍物影响距离 (m) |
| `attractive_gain` | 1.0 | 吸引力增益 |
| `repulsive_gain` | 100.0 | 排斥力增益 |
| `angular_gain` | 2.0 | 角度到角速度的增益 |

### 速度控制示例

```bash
# 发布前进指令
ros2 topic pub /diff_drive_controller/cmd_vel_unstamped geometry_msgs/Twist "{linear: {x: 0.5}, angular: {z: 0.0}}"
```

## 功能包说明

### diff_robot_description

机器人描述文件包，包含：
- URDF/xacro 机器人模型（底盘、差速轮、万向轮）
- RViz 配置文件

### diff_robot_control

仿真与控制包，包含：
- Gazebo 仿真世界和启动文件
- ros2_control 控制器配置
- 差速驱动控制器（DiffDriveController）+ 关节状态广播器

### diff_robot_teleop

遥控节点包，包含：
- 键盘遥控节点（支持速度动态调节）
- 可配置的速度上下限与话题名称

### diff_robot_navigation

导航与避障功能包，包含：
- 基于人工势场法（APF）的自动避障节点
- 激光雷达数据实时处理
- 可配置的安全距离、影响力范围及增益参数

## 许可证

Apache License 2.0