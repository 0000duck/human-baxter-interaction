#include "ros/ros.h"
#include "skeleton_msg/Skeleton.h"


void chatterCallback(const skeleton_msg::Skeleton::ConstPtr& msg)
{
  ROS_INFO("I heard: [%f,%f,%f,%f]", msg->skeleton[0],msg->skeleton[1],msg->skeleton[2],msg->skeleton[3]);
}

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("cmd_vel", 10000, chatterCallback);

  ros::spin();

  return 0;
}