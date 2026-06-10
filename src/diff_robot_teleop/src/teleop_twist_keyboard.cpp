/**
 * @file teleop_twist_keyboard.cpp
 * @brief 差速驱动机器人键盘遥控节点
 */

#include "diff_robot_teleop/teleop_twist_keyboard.hpp"

namespace diff_robot_teleop
{

TeleopTwistKeyboard::TeleopTwistKeyboard(const rclcpp::NodeOptions & options)
: Node("teleop_twist_keyboard", options)
{
  this->declare_parameter("linear_speed", 0.5);
  this->declare_parameter("angular_speed", 1.0);
  this->declare_parameter("cmd_vel_topic", std::string("/diff_drive_controller/cmd_vel_unstamped"));
  this->declare_parameter("max_linear_speed", 2.0);
  this->declare_parameter("max_angular_speed", 4.0);

  linear_speed_ = this->get_parameter("linear_speed").as_double();
  angular_speed_ = this->get_parameter("angular_speed").as_double();
  std::string cmd_vel_topic = this->get_parameter("cmd_vel_topic").as_string();
  max_linear_speed_ = this->get_parameter("max_linear_speed").as_double();
  max_angular_speed_ = this->get_parameter("max_angular_speed").as_double();

  publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(cmd_vel_topic, 10);

  RCLCPP_INFO(this->get_logger(), "键盘遥控节点已启动");
  RCLCPP_INFO(this->get_logger(), "线速度: %.2f m/s", linear_speed_);
  RCLCPP_INFO(this->get_logger(), "角速度: %.2f rad/s", angular_speed_);
  RCLCPP_INFO(this->get_logger(), "发布话题: %s", cmd_vel_topic.c_str());
}

TeleopTwistKeyboard::~TeleopTwistKeyboard()
{
  stop();
}

void TeleopTwistKeyboard::publishTwist(double linear_x, double angular_z)
{
  auto twist = geometry_msgs::msg::Twist();
  twist.linear.x = linear_x;
  twist.angular.z = angular_z;
  publisher_->publish(twist);
}

void TeleopTwistKeyboard::stop()
{
  publishTwist(0.0, 0.0);
}

}  // namespace diff_robot_teleop