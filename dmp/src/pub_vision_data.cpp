#include "ros/ros.h"
#include <iostream>
#include <std_msgs/Float32MultiArray.h>
#include "std_msgs/String.h"

#include <sstream>

int main(int argc, char **argv)
{

  ros::init(argc, argv, "talker");

  ros::NodeHandle n;


  ros::Publisher chatter_pub = n.advertise<std_msgs::Float32MultiArray>("/visioninfo", 1);

  ros::Rate loop_rate(10);
  std::cout<<"ready"<<std::endl;
  int count = 0;
  std_msgs::Float32MultiArray msg;
  msg.data = {1,2,3,4};
  while (ros::ok())
  {
    chatter_pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
    for(int i=0; i!=msg.data.size(); ++i)
      msg.data[i] += 1;
  }


  return 0;
}