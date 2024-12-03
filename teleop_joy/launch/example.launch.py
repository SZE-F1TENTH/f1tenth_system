from launch import LaunchDescription
import launch.actions
from launch_ros.actions import Node

def generate_launch_description():
    ld = LaunchDescription([
        launch.actions.DeclareLaunchArgument('cmd_vel')
    ])
    
    teleop_joy_node = Node(
        package='teleop_joy',
        executable='teleop_joy',
        name='teleop_joy',
        output='screen'
    )

    ld.add_action(teleop_joy_node)

    return ld