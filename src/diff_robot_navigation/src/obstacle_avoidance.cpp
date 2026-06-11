/**
 * @file obstacle_avoidance.cpp
 * @brief 差速驱动机器人避障节点实现（APF）
 */

#include "diff_robot_navigation/obstacle_avoidance.hpp"

#include <algorithm>
#include <cmath>

namespace diff_robot_navigation
{

ObstacleAvoidance::ObstacleAvoidance(const rclcpp::NodeOptions & options)
: Node("obstacle_avoidance", options)
{
  this->declare_parameter("linear_speed", 0.3);
  this->declare_parameter("angular_speed", 0.8);
  this->declare_parameter("safety_distance", 0.3);
  this->declare_parameter("influence_distance", 1.5);
  this->declare_parameter("attractive_gain", 1.0);
  this->declare_parameter("repulsive_gain", 100.0);
  this->declare_parameter("scan_topic", std::string("/scan"));
  this->declare_parameter("cmd_vel_topic", std::string("/diff_drive_controller/cmd_vel_unstamped"));

  linear_speed_ = this->get_parameter("linear_speed").as_double();
  angular_speed_ = this->get_parameter("angular_speed").as_double();
  safety_distance_ = this->get_parameter("safety_distance").as_double();
  influence_distance_ = this->get_parameter("influence_distance").as_double();
  attractive_gain_ = this->get_parameter("attractive_gain").as_double();
  repulsive_gain_ = this->get_parameter("repulsive_gain").as_double();
  scan_topic_ = this->get_parameter("scan_topic").as_string();
  cmd_vel_topic_ = this->get_parameter("cmd_vel_topic").as_string();

  scan_subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
    scan_topic_, 10,
    std::bind(&ObstacleAvoidance::scanCallback, this, std::placeholders::_1));

  cmd_vel_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(cmd_vel_topic_, 10);

  RCLCPP_INFO(this->get_logger(), "势场法避障节点已启动");
  RCLCPP_INFO(this->get_logger(), "最大线速度: %.2f m/s", linear_speed_);
  RCLCPP_INFO(this->get_logger(), "最大角速度: %.2f rad/s", angular_speed_);
  RCLCPP_INFO(this->get_logger(), "安全距离: %.2f m", safety_distance_);
  RCLCPP_INFO(this->get_logger(), "影响距离: %.2f m", influence_distance_);
  RCLCPP_INFO(this->get_logger(), "吸引力增益: %.2f", attractive_gain_);
  RCLCPP_INFO(this->get_logger(), "排斥力增益: %.2f", repulsive_gain_);
  RCLCPP_INFO(this->get_logger(), "订阅话题: %s", scan_topic_.c_str());
  RCLCPP_INFO(this->get_logger(), "发布话题: %s", cmd_vel_topic_.c_str());
}

ObstacleAvoidance::~ObstacleAvoidance()
{
  stop();
}

void ObstacleAvoidance::scanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
  processAvoidance(msg);
}

void ObstacleAvoidance::processAvoidance(const sensor_msgs::msg::LaserScan::SharedPtr scan)
{
  Vector2D attractive_force = calculateAttractiveForce();
  Vector2D repulsive_force = calculateRepulsiveForce(scan);
  Vector2D total_force = attractive_force + repulsive_force;

  float min_distance = scan->range_max;
  for (const auto & range : scan->ranges) {
    if (std::isfinite(range) && range > scan->range_min) {
      min_distance = std::min(min_distance, range);
    }
  }

  if (min_distance < safety_distance_) {
    RCLCPP_WARN(this->get_logger(), "检测到障碍物距离 %.2f m，紧急停止", min_distance);
    stop();
    return;
  }

  double linear_x = 0.0;
  double angular_z = 0.0;
  forceToVelocity(total_force, linear_x, angular_z);
  publishTwist(linear_x, angular_z);

  RCLCPP_DEBUG(this->get_logger(),
    "合力: (%.2f, %.2f), 速度: (%.2f, %.2f)",
    total_force.x, total_force.y, linear_x, angular_z);
}

Vector2D ObstacleAvoidance::calculateAttractiveForce() const
{
  // 假设目标在正前方，吸引力指向正前方
  // 吸引力 = 增益 * (目标位置 - 当前位置)
  // 简化为：吸引力 = 增益 * 单位向量(1, 0)
  return Vector2D(attractive_gain_, 0.0);
}

Vector2D ObstacleAvoidance::calculateRepulsiveForce(
  const sensor_msgs::msg::LaserScan::SharedPtr scan) const
{
  Vector2D total_repulsive(0.0, 0.0);

  for (size_t i = 0; i < scan->ranges.size(); ++i) {
    float range = scan->ranges[i];

    if (!std::isfinite(range) || range < scan->range_min || range > scan->range_max) {
      continue;
    }

    if (range > influence_distance_) {
      continue;
    }

    double angle = scan->angle_min + i * scan->angle_increment;
    double obstacle_x = range * std::cos(angle);
    double obstacle_y = range * std::sin(angle);

    // 排斥力 = 增益 * (1/距离 - 1/影响距离) * (1/距离^2)
    double distance = range;
    double repulsive_magnitude = repulsive_gain_ *
      (1.0 / distance - 1.0 / influence_distance_) / (distance * distance);

    // 排斥力方向：从障碍物指向机器人（即远离障碍物）
    // 在机器人坐标系中，方向为 (-obstacle_x, -obstacle_y) 的单位向量
    Vector2D direction(-obstacle_x, -obstacle_y);
    Vector2D normalized_dir = direction.normalized();

    // 累加排斥力
    total_repulsive = total_repulsive + normalized_dir * repulsive_magnitude;
  }

  return total_repulsive;
}

void ObstacleAvoidance::forceToVelocity(
  const Vector2D & total_force, double & linear_x, double & angular_z)
{
  // 如果合力很小，停止
  if (total_force.magnitude() < 0.01) {
    linear_x = 0.0;
    angular_z = 0.0;
    return;
  }

  // 线速度：合力的x分量（前进方向）
  // 限制在最大线速度范围内
  linear_x = std::max(-linear_speed_, std::min(linear_speed_, total_force.x));

  // 角速度：根据合力的y分量（侧向）计算转向
  // 使用atan2计算期望角度，然后转换为角速度
  double desired_angle = std::atan2(total_force.y, total_force.x);

  // 简单的比例控制：角速度 = 期望角度 * 增益
  // 限制在最大角速度范围内
  double angular_gain = 2.0;  // 角度到角速度的增益
  angular_z = std::max(-angular_speed_, std::min(angular_speed_, desired_angle * angular_gain));

  // 如果线速度为负（后退），反转角速度方向
  if (linear_x < 0) {
    angular_z = -angular_z;
  }
}

void ObstacleAvoidance::publishTwist(double linear_x, double angular_z)
{
  auto twist = geometry_msgs::msg::Twist();
  twist.linear.x = linear_x;
  twist.angular.z = angular_z;
  cmd_vel_publisher_->publish(twist);
}

void ObstacleAvoidance::stop()
{
  publishTwist(0.0, 0.0);
}

}  // namespace diff_robot_navigation
