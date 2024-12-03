cmake_minimum_required(VERSION 3.5)
project(teleop_joy)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(ackermann_msgs REQUIRED)
add_executable(teleop_joy src/teleop_joy.cpp)
ament_target_dependencies(teleop_joy rclcpp sensor_msgs ackermann_msgs)

install(TARGETS
  teleop_joy
  DESTINATION lib/${PROJECT_NAME})

ament_package()