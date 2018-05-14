#include "ros/ros.h"
#include <iostream>
#include "dmp/getWeight.h"

using namespace std;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "get_weight_client");

  if (argc != 3)
  {
    ROS_INFO("\nPlease input the path of the readFile and writeFile for example:\n/home/lancer/ros_ws/readFileName   /home/lancer/ros_ws/writeFileName\n");
    return 1;
  }

  ros::NodeHandle n;
  ros::ServiceClient client = n.serviceClient<dmp::getWeight>("get_weight");

  dmp::getWeight srv;
  srv.request.readFile = argv[1];
  srv.request.writeFile = argv[2];
  if (client.call(srv))
  {
    cout<<"\nsuccess save to:"<<srv.response.weight<<"\n"<<endl;
  }
  else
  {
    ROS_ERROR("Failed to call service get_weight");
    return 1;
  }

  return 0;
}
