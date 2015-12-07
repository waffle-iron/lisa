#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/UInt64.h>

#define WHEEL_ENCODER_M_DISTANCE_FROM_TICKS 0.0544737

double g_distance = 0.0;
unsigned int g_ticks = 0;
geometry_msgs::Twist g_twist;

void twistCallback(const geometry_msgs::Twist& msg) {
  g_twist = msg;
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "wheel_encoder_simulator");
  ROS_INFO("wheel_encoder_simulator started");
  ros::NodeHandle n;

  ros::Publisher wheel_encoder_pub = n.advertise<std_msgs::UInt64>("lisa/sensors/wheel_encoder", 1);
  ros::Subscriber twist_sub = n.subscribe("lisa/twist", 1, twistCallback);

  // Initialize time stamp
  ros::Time stamp = ros::Time::now();

  // Loop to publish wheel encoder updates as they should occur
  ros::Rate loop_rate(10); // Hz
  while (ros::ok())
  {
    ros::Time new_stamp = ros::Time::now();
    double dt = (new_stamp - stamp).toSec();
    stamp = new_stamp;

    // Update distance based on current twist if we have linear velocity
    double linear = g_twist.linear.x;
    g_distance += linear * dt;
    // Calculate new IMU ticks (rounded)
    unsigned int ticks = int(g_distance / WHEEL_ENCODER_M_DISTANCE_FROM_TICKS);

    // Report if ticks changed only
    if (ticks > g_ticks) {
      g_ticks = ticks;

      std_msgs::UInt64 msg;
      msg.data = ticks;
      wheel_encoder_pub.publish(msg);
      ROS_INFO("[WHEEL ENCODER SIMULATOR] ticks=%d (dt=%0.3fs, d=%0.3fm)", g_ticks, dt, g_distance);
    }

    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}
