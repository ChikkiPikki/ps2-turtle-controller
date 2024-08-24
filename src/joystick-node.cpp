#include <iostream>
#include <unistd.h>
#include <thread>
#include "../include/ps2_turtle_controller/joystick.cc" // external library, https://github.com/drewnoakes/joystick

// Create a ros2 publisher for publishing the joystick data to the turtle node

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.h>
#include <turtlesim/srv/set_pen.h>
using namespace std::chrono_literals;

Joystick      joystick;
JoystickEvent event;
struct WhereTo {
      float x;
      float y;
      float z;
      float x_a;
      float y_a;
      float z_a;
    } where;

int pen_state;

class PS2Node : public rclcpp::Node
{
  public:
     PS2Node()
    : Node("joystick_node"), count_(0)
    {
      vel_publisher = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
      vel_publisher = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    timer_ = this->create_wall_timer(
      2ms, std::bind(&PS2Node::change_vel, this));
    }
    
    void change_vel()
    {
      auto message = geometry_msgs::msg::Twist();
      message.linear.set__x(where.x);
      message.linear.set__y(where.y);
      message.linear.set__z(where.z);
      
      message.angular.set__x(where.x_a);
      message.angular.set__y(where.y_a);
      message.angular.set__z(where.z_a);

      vel_publisher->publish(message);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_publisher;
    size_t count_;
};

auto ps2Node = std::make_shared<PS2Node>;

void DetectControllerChange();
void axisPosChanged(int, int); // call everytime an axis position is changed
void buttonX(int);
void buttonSquare(int);
void buttonCircle(int);
void buttonTriangle(int);

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  std::thread ControllerChange(DetectControllerChange);
  rclcpp::spin(ps2Node());

  rclcpp::shutdown();
  ControllerChange.join();

  return 0;
}



void DetectControllerChange()
{

    while (true)
    {
        usleep(10000);

        if (joystick.sample(&event))
        {
            if (event.isButton())
            {
                // event.value contains the state of the button (up or down)
                switch (event.number)
                {
                  case 0: 
                    buttonTriangle(event.value);
                    break;
                  case 1:
                    buttonCircle(event.value);
                    break;
                  case 2:
                    buttonX(event.value);
                    break;
                  case 3:
                    buttonSquare(event.value);
                    break;
                  default:
                    printf("None");
                }
            }
            else if (event.isAxis())
            {
                // event.value contains the position of the axis between -32767 to 32767
                axisPosChanged(event.number, event.value);
            }
        }
    }
}

void axisPosChanged(int axis, int value){
  switch (axis)
  {
    case 0:
      where.x = value/10000;      
      break;
    case 1:
      where.y = -value/10000;
      break;
    case 2:
      where.x_a = value/100;
      break;
    case 3:
      where.y_a = value/100;
    default:
      break;
  }
}; // call everytime an axis position is changed


void buttonX(int value){
};
void buttonSquare(int value){

};
void buttonCircle(int value){

};
void buttonTriangle(int value){

};