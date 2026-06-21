/**
 * @file obstacle_avoidance_main.cpp
 * @brief 差速驱动机器人避障节点主程序
 */

#include "diff_robot_navigation/obstacle_avoidance.hpp"
#include <signal.h>
#include <memory>

namespace
{

std::shared_ptr<diff_robot_navigation::ObstacleAvoidance> g_node = nullptr;

void signalHandler(int signum)
{
  (void)signum;
  if (g_node) {
    g_node->stop();
  }
  rclcpp::shutdown();
  exit(0);
}

}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  g_node = std::make_shared<diff_robot_navigation::ObstacleAvoidance>();

  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  rclcpp::spin(g_node);

  if (g_node) {
    g_node->stop();
  }

  rclcpp::shutdown();
  return 0;
}