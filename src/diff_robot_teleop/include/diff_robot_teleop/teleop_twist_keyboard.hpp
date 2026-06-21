/**
 * @file teleop_twist_keyboard.hpp
 * @brief 差速驱动机器人键盘遥控节点
 */

#ifndef DIFF_ROBOT_TELEOP__TELEOP_TWIST_KEYBOARD_HPP_
#define DIFF_ROBOT_TELEOP__TELEOP_TWIST_KEYBOARD_HPP_

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <string>
#include <map>
#include <utility>

namespace diff_robot_teleop
{

/**
 * @class TeleopTwistKeyboard
 * @brief 键盘遥控节点，通过键盘输入控制机器人移动
 */

class TeleopTwistKeyboard : public rclcpp::Node
{
public:
  explicit TeleopTwistKeyboard(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

  ~TeleopTwistKeyboard() override;
  void publishTwist(double linear_x, double angular_z);
  void stop();

  double getLinearSpeed() const { return linear_speed_; }
  double getAngularSpeed() const { return angular_speed_; }
  double getMaxLinearSpeed() const { return max_linear_speed_; }
  double getMaxAngularSpeed() const { return max_angular_speed_; }
  void setLinearSpeed(double speed) { linear_speed_ = speed; }
  void setAngularSpeed(double speed) { angular_speed_ = speed; }

private:
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;

  double linear_speed_;
  double angular_speed_;
  double max_linear_speed_;
  double max_angular_speed_;
};

}  // namespace diff_robot_teleop

#endif  // DIFF_ROBOT_TELEOP__TELEOP_TWIST_KEYBOARD_HPP_