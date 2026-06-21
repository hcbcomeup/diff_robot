from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    pkg_share = FindPackageShare(package='diff_robot_navigation').find('diff_robot_navigation')
    
    use_sim_time = LaunchConfiguration('use_sim_time')
    config_file = LaunchConfiguration('config_file')
    
    default_config = pkg_share + '/config/obstacle_avoidance.yaml'
    
    return LaunchDescription([
        DeclareLaunchArgument(
            'use_sim_time',
            default_value='true',
            description='使用仿真时间'
        ),
        
        DeclareLaunchArgument(
            'config_file',
            default_value=default_config,
            description='避障参数配置文件路径'
        ),
        
        Node(
            package='diff_robot_navigation',
            executable='obstacle_avoidance',
            name='obstacle_avoidance',
            output='screen',
            parameters=[config_file, {'use_sim_time': use_sim_time}]
        ),
    ])