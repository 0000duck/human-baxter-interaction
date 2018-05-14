#include <iostream>
#include "ros/ros.h"
#include "dmp/integration.h"
// #include "geometry_msgs/Pose.h"

using namespace std;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "integration_client");
  ros::NodeHandle n;


  dmp::integration srv;

  dmp::TrajectoryData tt;
  
  string read = "19.30ww";
  string write = "19.30ttt";
  float time = 3;

  geometry_msgs::Pose s,g;
  geometry_msgs::Point ss;
  geometry_msgs::Quaternion qq;
  ss.x = 1;
  ss.y = 2;
  ss.z = 3;
  s.position = ss;
  qq.x = 1;
  qq.y = 2;
  qq.z = 1;
  qq.w = 0.5;
  s.orientation = qq;
  g = s;

  tt.readFile = read;
  tt.writeFile = write;
  tt.duration = time;
  tt.startPoint = s;
  tt.endPoint = g;
  cout<<tt<<endl;

  ros::ServiceClient client = n.serviceClient<dmp::integration>("get_integration");


  // = {read, write, time, s, g};
  // s.orientation = {1,2,3,4}; 


  
  // srv.test.readFile = "19.30ww";
  // srv.test.writeFile = "19.30ttt";
  // srv.test.duration = 3;
  // srv.test.startPoint;
  // srv.request.test = 
}