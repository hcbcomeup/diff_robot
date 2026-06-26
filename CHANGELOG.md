# 变更日志 (Changelog)

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/))
版本号遵循 [语义化版本](https://semver.org/lang/zh-CN/))

## [Unreleased]

### Added
- 初始项目结构搭建
- diff_robot_description: 机器人模型描述包（URDF/xacro、meshes、RViz配置）
- diff_robot_control: Gazebo仿真与ros2_control控制器配置
- diff_robot_teleop: 键盘遥控节点
- 项目文档：README
- 全局脚本：一键编译、环境配置
- diff_robot_navigation: 新增导航与避障功能包，基于势场法（APF）实现自动避障
- docker: 新增 Dockerfile、docker-compose.yaml、entrypoint.sh，支持容器化编译与运行

### Fixed
- env_setup.sh: 修复帮助信息中遗漏 `diff_robot_navigation` 避障导航命令的问题，新增 `ros2 launch diff_robot_navigation navigation.launch.py`
- gazebo.launch.py: 删除未使用的 `controller_config` 变量及未使用的导入（`IncludeLaunchDescription`、`PythonLaunchDescriptionSource`、`PathJoinSubstitution`）
- teleop_twist_keyboard.hpp: 删除未使用的 `<string>`、`<map>`、`<utility>` 头文件
- entrypoint.sh: 添加缺失的 `#!/bin/bash` shebang
- diff_robot_teleop: 修复速度限制硬编码问题，速度上下限现在使用 `max_linear_speed_` / `max_angular_speed_` 成员变量，支持通过参数动态调整
- diff_robot_control: 修复 Gazebo launch 文件中 `joint_state_publisher` 与 `joint_state_broadcaster` 同时运行导致的 `/joint_states` 话题冲突
- build_all.sh: 修复参数解析 `for arg in "$@"` 内使用 `shift` 无效的 bug，改为 `while [ $# -gt 0 ]` 循环
- diff_robot_description: 修复万向轮 `caster_joint` 使用 `type="fixed"` 导致无法滑动的问题，改为 `type="continuous"` 并添加阻尼与摩擦配置
- diff_robot_description: 修复 URDF 缺少 `<ros2_control>` 标签和 `libgazebo_ros2_control.so` 插件，导致 controller_manager 无法启动、spawner 持续等待 `/controller_manager/list_controllers` 服务、仿真机器人不响应 /cmd_vel 指令的问题

### Changed
- diff_robot_teleop: 重构代码结构，实现节点功能与主循环逻辑的分离
- README: 完善文档内容，修正话题名称、新增导航避障使用说明、键盘遥控按键表、避障参数表及 Docker 运行指南

### Performance
- diff_robot_navigation: `calculateRepulsiveForce()` 重复除法优化，预计算 `inv_distance` 和 `inv_influence`，以乘法替代除法，减少 LiDAR 每帧数百点的浮点运算开销
- diff_robot_teleop: `getKey()` 函数参数由按值传递 `struct termios` 改为 const 引用传递，避免每次调用复制 ~60 字节
- diff_robot_navigation: `calculateAttractiveForce()` 常量缓存优化，吸引力向量只计算一次并缓存，避免重复构造相同对象
- diff_robot_navigation: `forceToVelocity()` 中 `angular_gain` 由硬编码 `2.0` 改为可配置参数，默认值保持不变，新增 `angular_gain` 字段到 YAML 配置文件