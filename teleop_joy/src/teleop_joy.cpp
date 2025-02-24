#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"

class JoyTeleopNode : public rclcpp::Node
{
public:
    JoyTeleopNode() : Node("teleop_joy"), use_teleop_(true)
    {
        joy_subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy", 10, std::bind(&JoyTeleopNode::joy_callback, this, std::placeholders::_1));

        teleop_publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>(
            "teleop", 10);

        ackermann_cmd_publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>(
            "ackermann_cmd", 10);

        // Initially subscribe to the /teleop topic
        subscribe_to_teleop();
    }

private:
    void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg)
    {
        auto teleop_msg = ackermann_msgs::msg::AckermannDriveStamped();
        
        if (msg->buttons[4] == 1)
        {
            subscribe_to_drive();
            RCLCPP_INFO(this->get_logger(), "Switched to %s mode", "Autonomous Mode");
        }
        else
        {
            subscribe_to_teleop();

            teleop_msg.drive.speed = msg->axes[1];
            teleop_msg.drive.steering_angle = msg->axes[3]*5;
            ackermann_cmd_publisher_->publish(teleop_msg);
            teleop_publisher_->publish(teleop_msg);
        }
        // if (msg->buttons[2] == 1)
        // {
        //     std::cout << "You are Now getting to recording mode : Press (Y accept /N decline) " << std::endl;
        //     char response;
        //     std::cin >> response;
        //     if (response == 'Y' || response == 'y')
        //     {
            
        //         std::string command = "ros2 bag record -s mcap --all ";
        //         system(command.c_str());
        //     }
        //     else if (response == 'N' || response == 'n')
        //     {
        //         std::cout << "You have declined the recording mode" << std::endl;
        //     }
        // }
        if (msg->buttons[3] == 1) // Assuming button 3 is the kill switch
        {
            subscribe_to_teleop();
            RCLCPP_WARN(this->get_logger(), "Kill switch activated, shutting down...");
            //std::system("killall -9 ros2");
            std:system("pkill -f '/opt/ros/foxy'"
        }
    }
    void subscribe_to_drive()
    {
        drive_subscription_ = this->create_subscription<ackermann_msgs::msg::AckermannDriveStamped>(
            "drive", 1, std::bind(&JoyTeleopNode::drive_callback, this, std::placeholders::_1));
        subscribed_to_drive_ = true;
        RCLCPP_INFO(this->get_logger(), "Subscribed to /drive topic");

        // Start a timer to check for drive messages
        drive_timer_ = this->create_wall_timer(
            std::chrono::seconds(5), std::bind(&JoyTeleopNode::check_drive_timeout, this));
    }

    void subscribe_to_teleop()
    {
        drive_subscription_.reset();
        subscribed_to_drive_ = false;
        RCLCPP_INFO(this->get_logger(), "Unsubscribed from /drive topic");
    }

    void drive_callback(const ackermann_msgs::msg::AckermannDriveStamped::SharedPtr msg)
    {
        // Reset the timer whenever a drive message is received
        last_drive_msg_time_ = this->now();
    }

    void check_drive_timeout()
    {
        if (subscribed_to_drive_ && (this->now() - last_drive_msg_time_).seconds() > 5.0)
        {
            RCLCPP_WARN(this->get_logger(), "No drive messages received, switching back to teleop mode");
            subscribe_to_teleop();
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_subscription_;
    rclcpp::Subscription<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr drive_subscription_;
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr teleop_publisher_;
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr ackermann_cmd_publisher_;
    rclcpp::TimerBase::SharedPtr drive_timer_;
    rclcpp::Time last_drive_msg_time_;
    bool use_teleop_;
    bool subscribed_to_drive_ = false;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<JoyTeleopNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
