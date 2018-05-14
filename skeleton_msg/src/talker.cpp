#include "ros/ros.h"
#include "string.h"
#include "skeleton_msg/Skeleton.h"

#include <sstream>


int main(int argc, char **argv)
{

  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<skeleton_msg::Skeleton>("chatter", 10000);
  ros::Rate loop_rate(10);
  int int_a = 0;
  int count = 0;
  while (ros::ok())
  {
    
    skeleton_msg::Skeleton msg;
    std::stringstream ss;
    ss << "hello world " << count;
    
    for(int i=0;i<75; i++){
      msg.skeleton[i] = int_a;
    }
    int_a++;

    ROS_INFO("%f", msg.skeleton[0]);
    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
  }


  return 0;
}