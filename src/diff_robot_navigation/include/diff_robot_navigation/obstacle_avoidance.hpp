/**
 * @file obstacle_avoidance.hpp
 * @brief 差速驱动机器人避障节点
 */

#ifndef DIFF_ROBOT_NAVIGATION__OBSTACLE_AVOIDANCE_HPP_
#define DIFF_ROBOT_NAVIGATION__OBSTACLE_AVOIDANCE_HPP_

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <string>

namespace diff_robot_navigation
{

struct Vector2D
{
  double x;
  double y;

  Vector2D() : x(0.0), y(0.0) {}
  Vector2D(double x_, double y_) : x(x_), y(y_) {}

  Vector2D operator+(const Vector2D & other) const
  {
    return Vector2D(x + other.x, y + other.y);
  }

  Vector2D operator*(double scalar) const
  {
    return Vector2D(x * scalar, y * scalar);
  }

  double magnitude() const
  {
    return std::sqrt(x * x + y * y);
  }

  Vector2D normalized() const
  {
    double mag = magnitude();
    if (mag > 1e-6) {
      return Vector2D(x / mag, y / mag);
    }
    return Vector2D(0.0, 0.0);
  }
};

/**
 * @class ObstacleAvoidance
 * @brief 避障节点，使用势场法（APF）实现自动避障
 */
class ObstacleAvoidance : public rclcpp::Node
{
public:
  explicit ObstacleAvoidance(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

  ~ObstacleAvoidance() override;

  void publishTwist(double linear_x, double angular_z);
  void stop();
  double getLinearSpeed() const { return linear_speed_; }
  double getAngularSpeed() const { return angular_speed_; }
  void setLinearSpeed(double speed) { linear_speed_ = speed; }
  void setAngularSpeed(double speed) { angular_speed_ = speed; }

protected:
  void scanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg);
  void processAvoidance(const sensor_msgs::msg::LaserScan::SharedPtr scan);
  Vector2D calculateAttractiveForce() const;
  Vector2D calculateRepulsiveForce(const sensor_msgs::msg::LaserScan::SharedPtr scan) const;
  void forceToVelocity(const Vector2D & total_force, double & linear_x, double & angular_z);

private:
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_subscriber_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;

  double linear_speed_;
  double angular_speed_;
  double safety_distance_;
  double influence_distance_;
  double attractive_gain_;
  double repulsive_gain_;
  double angular_gain_;

  std::string scan_topic_;
  std::string cmd_vel_topic_;

  mutable Vector2D cached_attractive_force_;
  mutable bool attractive_force_valid_;
};

}  // namespace diff_robot_navigation

#endif  // DIFF_ROBOT_NAVIGATION__OBSTACLE_AVOIDANCE_HPP_