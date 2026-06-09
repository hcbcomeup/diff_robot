from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    pkg_share = FindPackageShare(package='diff_robot_description').find('diff_robot_description')
    
    xacro_file = LaunchConfiguration('xacro_file')
    
    robot_description = Command(['xacro ', xacro_file])
    
    return LaunchDescription([
        DeclareLaunchArgument(
            'xacro_file',
            default_value=pkg_share + '/urdf/diff_robot.urdf.xacro',
            description='机器人URDF/xacro文件路径'
        ),
        
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{'robot_description': robot_description}]
        ),
        
        Node(
            package='joint_state_publisher_gui',
            executable='joint_state_publisher_gui',
            name='joint_state_publisher_gui'
        ),
        
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen'
        )
    ])
