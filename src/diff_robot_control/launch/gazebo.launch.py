from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    pkg_share = FindPackageShare(package='diff_robot_control').find('diff_robot_control')
    pkg_description = FindPackageShare(package='diff_robot_description').find('diff_robot_description')
    
    use_sim_time = LaunchConfiguration('use_sim_time')
    world_file = LaunchConfiguration('world')
    
    xacro_file = pkg_description + '/urdf/diff_robot.urdf.xacro'
    robot_description = Command(['xacro ', xacro_file])
    
    controller_config = pkg_share + '/config/diff_robot_controllers.yaml'
    
    return LaunchDescription([
        DeclareLaunchArgument(
            'use_sim_time',
            default_value='true',
            description='使用仿真时间'
        ),
        
        DeclareLaunchArgument(
            'world',
            default_value=pkg_share + '/worlds/empty.world',
            description='Gazebo世界文件路径'
        ),
        
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{
                'robot_description': robot_description,
                'use_sim_time': use_sim_time
            }]
        ),
        
        ExecuteProcess(
            cmd=['gazebo', '--verbose', world_file, '-s', 'libgazebo_ros_factory.so'],
            output='screen'
        ),
        
        Node(
            package='gazebo_ros',
            executable='spawn_entity.py',
            name='spawn_robot',
            arguments=['-topic', 'robot_description', '-entity', 'diff_robot'],
            output='screen'
        ),
        
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['joint_state_broadcaster', '--controller-manager', '/controller_manager'],
            output='screen'
        ),
        
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['diff_drive_controller', '--controller-manager', '/controller_manager'],
            output='screen'
        ),
    ])