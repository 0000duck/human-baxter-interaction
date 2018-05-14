#include <iostream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32MultiArray.h"
/////////////////////////////////////////////////////////////////////////
#include <iomanip>
#include <tf/transform_broadcaster.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "dmp/function.h"
#include "dmp/getTrajectory.h"
using namespace Eigen;
/////////////////////////////////////////////////////////////////////////
using namespace std;
int count ;

void getNewPoint(dmp::TrajectoryData &trajectory, float time, string weightFile, Quaternion<double,AutoAlign> qua)
{
  MatrixXd w_t = readTxt(weightFile);
  VectorXd oldIni = w_t.row(w_t.rows()-3);
  VectorXd oldGoal = w_t.row(w_t.rows()-2);

  Vector3d oldStartPoint = oldIni.topRows(3);
  Vector4d midTest = oldIni.bottomRows(4);
  Quaternion<double,AutoAlign> oldStartQua( midTest ); 
  Vector3d oldEndPoint = oldGoal.topRows(3);
  midTest = oldGoal.bottomRows(4);
  Quaternion<double,AutoAlign> oldEndQua( midTest );

  Vector3d startPoint = qua * oldStartPoint;
  Quaternion<double,AutoAlign> startQua = qua * oldStartQua;

  Vector3d endPoint = qua * oldEndPoint;
  Quaternion<double,AutoAlign> endQua = qua * oldEndQua;

  geometry_msgs::Pose s,g;
  geometry_msgs::Point ss;
  geometry_msgs::Quaternion qq;

  ss.x = startPoint(0);
  ss.y = startPoint(1);
  ss.z = startPoint(2);
  s.position = ss;
  qq.x = startQua.x();
  qq.y = startQua.y();
  qq.z = startQua.z();
  qq.w = startQua.w();
  s.orientation = qq;

  ss.x = endPoint(0);
  ss.y = endPoint(1);
  ss.z = endPoint(2);
  g.position = ss;
  qq.x = endQua.x();
  qq.y = endQua.y();
  qq.z = endQua.z();
  qq.w = endQua.w();
  g.orientation = qq;

  trajectory.readFile = weightFile;
  trajectory.writeFile = weightFile+"tt";
  trajectory.duration = time;
  trajectory.startPoint = s;
  trajectory.endPoint = g;

}

void robot(dmp::TrajectoryData trajectory)
{
  dmp::getTrajectory srv;
  srv.request.inputData = trajectory;
  if (ros::service::call("get_trajectory", srv))
  {
    string aa = "cd ~/ros_ws/src/dmp/scripts; python execute_endpoint_data.py -f /home/lancer/ros_ws/";  
    string cc = " joint_states:=/robot/joint_states";
    string bb = trajectory.writeFile;
    string path = aa + bb + cc;
    const char *a = path.c_str();
    system(a);
    cout<<"\nsuccess to callback!"<<endl;
  }
  else
    cout<<"Failed to call service get_trajectory!"<<endl;
}

void chatterCallback(const std_msgs::Float32MultiArray& msg)
{
  ////////////////////////////////////////////
  int count = 0;//delete
  ////////////////////////////////////////////
  if(msg.data[0] == 1)
      ++count;
    else
      count = 0;
  
  float time = 3;
  tf::Quaternion rotation;
  float yall;

  if(count >= 10)
  {
    yall = atan( (360-msg.data[1]) / msg.data[2]);
    rotation.setRPY(0, 0, yall);
    Quaternion<double,AutoAlign> qua(rotation);

    string weightFile ; 
    if(msg.data[3]>=0.8)
      weightFile = "23.23w";
    else
      weightFile = "00.21w";

    dmp::TrajectoryData trajectory;
    getNewPoint(trajectory, time, weightFile, qua);
    robot(trajectory);
    sleep(5);
  }

}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "get_vision");

  ////////////////////////////////////////////
  int count = 0;//delete
  ////////////////////////////////////////////
  ros::NodeHandle n;


  ros::Subscriber sub = n.subscribe("/visioninfo", 1, chatterCallback);


  ros::spin();

  return 0;
}