/**
 * @file teleop_twist_keyboard.cpp
 * @brief 差速驱动机器人键盘遥控节点
 */

#include "diff_robot_teleop/teleop_twist_keyboard.hpp"

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#include <iostream>
#include <map>
#include <utility>

namespace diff_robot_teleop
{

const std::map<char, std::pair<double, double>> MOVE_BINDINGS = {
  {'w', {1.0, 0.0}},    // 前进
  {'x', {-1.0, 0.0}},   // 后退
  {'a', {0.0, 1.0}},    // 左转
  {'d', {0.0, -1.0}},   // 右转
  {'s', {0.0, 0.0}},    // 停止
};

// 速度调节按键映射: 按键 -> (线速度倍数, 角速度倍数)
const std::map<char, std::pair<double, double>> SPEED_BINDINGS = {
  {'q', {1.1, 1.0}},   // 增加线速度
  {'z', {0.9, 1.0}},   // 减少线速度
  {'e', {1.0, 1.1}},   // 增加角速度
  {'c', {1.0, 0.9}},   // 减少角速度
};

const char* HELP_MSG = R"(
差速驱动机器人键盘遥控
----------------------
移动控制:
    w : 前进
    x : 后退
    a : 左转
    d : 右转
    s : 停止

速度调节:
    q/z : 增加/减少线速度 (10%)
    e/c : 增加/减少角速度 (10%)

Ctrl+C : 退出
)";

char getKey(struct termios settings)
{
  struct termios raw = settings;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &raw);

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000;  // 100ms 超时

  char key = 0;
  if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
    read(STDIN_FILENO, &key, 1);
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &settings);
  return key;
}

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