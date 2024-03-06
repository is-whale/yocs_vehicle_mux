#include "ros/publisher.h"
#include "ros/subscriber.h"
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <autoware_msgs/VehicleCmd.h>
#include <autoware_msgs/ControlCommand.h>

class CmdVelToVehicleCmd {
public:
  CmdVelToVehicleCmd() {
        // output
        mux_output_sub = nh.subscribe("/cmd_vel", 1, &CmdVelToVehicleCmd::cmdVelCallback, this);
        mux_output_pub = nh.advertise<autoware_msgs::VehicleCmd>("/vehicle_cmd", 1);
        // autoware input
        autoware_vehicle_sub = nh.subscribe("/autoware",1,&CmdVelToVehicleCmd::autoware_vehicle_callback,this);        
        autoware_cmd_pub = nh.advertise<geometry_msgs::Twist>("/input/autoware_cmd",1);
    }

private:
    ros::NodeHandle nh;

    ros::Subscriber mux_output_sub;
    ros::Publisher  mux_output_pub;

    ros::Subscriber autoware_vehicle_sub;
    ros::Publisher autoware_cmd_pub;

    // ros::Publisher pub;
    void autoware_vehicle_callback(const autoware_msgs::VehicleCmd &msg) {
      geometry_msgs::Twist cmd_vel_pub;
      cmd_vel_pub.linear = msg.twist_cmd.twist.linear;
      cmd_vel_pub.angular = msg.twist_cmd.twist.angular;
      autoware_cmd_pub.publish(cmd_vel_pub);
      
    };

    void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
        // 创建VehicleCmd消息
        autoware_msgs::VehicleCmd vehicle_cmd;

        // 创建ControlCommand消息
        autoware_msgs::ControlCommand ctrl_cmd;
	double speed = msg->linear.x;
	//if(msg->linear.x < 0){
	//speed = 0;
	//}
        // 将Twist消息转化为ControlCommand消息
        ctrl_cmd.linear_velocity = speed;
        // ctrl_cmd.linear_acceleration = msg->linear.y;
        ctrl_cmd.steering_angle = msg->angular.z;

        // 将ControlCommand消息设置为VehicleCmd消息的一部分
        vehicle_cmd.ctrl_cmd = ctrl_cmd;

        // 发布VehicleCmd消息
        mux_output_pub.publish(vehicle_cmd);
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "cmd_vel_to_vehicle_cmd_node");
    CmdVelToVehicleCmd cmd_vel_to_vehicle_cmd;
    ros::spin();
    return 0;
}
