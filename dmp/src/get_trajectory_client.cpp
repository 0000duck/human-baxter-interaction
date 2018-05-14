#include "ros/ros.h"
#include <iostream>
#include <geometry_msgs/Pose.h>
#include "dmp/getTrajectory.h"

using namespace std;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "get_trajectory_client");
  if (argc < 4)
  {
    ROS_INFO("Please input the path of the readFile  writeFile and duration for example:\n/home/lancer/ros_ws/readFileName   /home/lancer/ros_ws/writeFileName   duration");
    return 1;
  }

  ros::NodeHandle n;
  ros::ServiceClient client = n.serviceClient<dmp::getTrajectory>("get_trajectory");

  dmp::getTrajectory srv;
  srv.request.inputData.readFile = argv[1];
  srv.request.inputData.writeFile = argv[2];
  srv.request.inputData.duration = atoll(argv[3]);

  srv.request.inputData.s_dx = 0.0;
  srv.request.inputData.s_dy = 0.0;
  srv.request.inputData.s_dz = 0.0;

  srv.request.inputData.e_dx = 0.0;
  srv.request.inputData.e_dy = 0.0;
  srv.request.inputData.e_dz = 0.0;

  
  if(argc == 7){
    srv.request.inputData.s_dx = stof(argv[4]);
    srv.request.inputData.s_dy = stof(argv[5]);
    srv.request.inputData.s_dz = stof(argv[6]);
  }else if(argc == 10){
    srv.request.inputData.s_dx = stof(argv[4]);
    srv.request.inputData.s_dy = stof(argv[5]);
    srv.request.inputData.s_dz = stof(argv[6]);
    srv.request.inputData.e_dx = stof(argv[7]);
    srv.request.inputData.e_dy = stof(argv[8]);
    srv.request.inputData.e_dz = stof(argv[9]);
  }

  if (client.call(srv))
  {
    cout<<"\nsuccess save to:"<<srv.response.trajectory<<srv.request.inputData.duration<<"\n"<<endl;
  }
  else
  {
    ROS_ERROR("Failed to call service get_trajectory");
    return 1;
  }

  return 0;
}
