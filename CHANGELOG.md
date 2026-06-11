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

### Changed
- diff_robot_teleop: 重构代码结构，实现节点功能与主循环逻辑的分离

### Added
- diff_robot_navigation: 新增导航与避障功能包,基于势场法（APF）实现自动避障