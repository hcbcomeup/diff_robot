/**
 * @file teleop_twist_keyboard_main.cpp
 * @brief 差速驱动机器人键盘遥控主程序
 */

#include "diff_robot_teleop/teleop_twist_keyboard.hpp"

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>

#include <iostream>
#include <map>
#include <utility>
#include <memory>

namespace
{

// 移动控制按键映射: 按键 -> (线速度方向, 角速度方向)
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

std::shared_ptr<diff_robot_teleop::TeleopTwistKeyboard> g_node = nullptr;

void signalHandler(int signum)
{
  (void)signum;
  if (g_node) {
    g_node->stop();
  }
  rclcpp::shutdown();
  exit(0);
}

char getKey(const struct termios & settings)
{
  struct termios raw = settings;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &raw);

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000;

  char key = 0;
  if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
    read(STDIN_FILENO, &key, 1);
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &settings);
  return key;
}

}  // namespace

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  g_node = std::make_shared<diff_robot_teleop::TeleopTwistKeyboard>();

  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  struct termios original_settings;
  tcgetattr(STDIN_FILENO, &original_settings);

  std::cout << HELP_MSG << std::endl;

  rclcpp::Rate rate(20);  // 20Hz 循环频率
  
  while (rclcpp::ok()) {
    char key = getKey(original_settings);

    if (key != 0) {
      auto move_it = MOVE_BINDINGS.find(key);
      if (move_it != MOVE_BINDINGS.end()) {
        double linear_x = move_it->second.first * g_node->getLinearSpeed();
        double angular_z = move_it->second.second * g_node->getAngularSpeed();
        g_node->publishTwist(linear_x, angular_z);
        
        if (key == 's') {
          std::cout << "停止" << std::endl;
        }
      }
      else {
        auto speed_it = SPEED_BINDINGS.find(key);
        if (speed_it != SPEED_BINDINGS.end()) {
          double new_linear_speed = g_node->getLinearSpeed() * speed_it->second.first;
          double new_angular_speed = g_node->getAngularSpeed() * speed_it->second.second;
          
          double max_linear = g_node->getMaxLinearSpeed();
          double max_angular = g_node->getMaxAngularSpeed();
          if (new_linear_speed > max_linear) new_linear_speed = max_linear;
          if (new_linear_speed < 0.1) new_linear_speed = 0.1;
          if (new_angular_speed > max_angular) new_angular_speed = max_angular;
          if (new_angular_speed < 0.1) new_angular_speed = 0.1;
          
          g_node->setLinearSpeed(new_linear_speed);
          g_node->setAngularSpeed(new_angular_speed);
          
          std::cout << "线速度: " << new_linear_speed << " m/s, "
                    << "角速度: " << new_angular_speed << " rad/s" << std::endl;
        }
      }
    }

    rclcpp::spin_some(g_node);
    rate.sleep();
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);

  if (g_node) {
    g_node->stop();
  }

  rclcpp::shutdown();
  return 0;
}