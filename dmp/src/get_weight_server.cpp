#include <iostream>
#include "ros/ros.h"
#include "dmp/function.h"
#include "dmp/getWeight.h"
#include "dmp/dmp.h"
#include <cstdlib>
//////////////////////////
#include <iomanip>
#include <tf/transform_broadcaster.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "dmp/function.h"
#include "dmp/getTrajectory.h"
using namespace Eigen;
//////////////////////////
using namespace std;


bool getWeights(dmp::getWeight::Request &req, dmp::getWeight::Response &res)
{
    res.weight = req.writeFile ;
    string readPath = req.readFile;
    string writePath = req.writeFile;
    int numKernel = 30;
    int alphaX = 1;
    double tau = 1;
    double alpha = 25;
    DMP obj(numKernel, alphaX, tau, alpha );
    obj.saveWeight(readPath, writePath);
    cout<<"\nThe weignt has already save to: "<<res.weight<<"\n"<<endl;
    return true;
}

int main(int argc, char *argv[])
{
/////////////////////////////////////////////////////////////////////  
    // string aa = "cd ~/ros_ws/src/dmp/scripts; python execute_endpoint_data.py -f ";  
    // string cc = " joint_states:=/robot/joint_states";
    // string bb = "/home/lancer/ros_ws/00.20t";
    // string path = aa + bb + cc;
    // const char *a = path.c_str();
    // system(a);
/////////////////////////////////////////////////////////////////////
    // geometry_msgs::Quaternion qq;
    // Quaternion<double,AutoAlign> endQua(1,2,3,4);
    // qq.x = endQua.x();
/////////////////////////////////////////////////////////////////////    
    ros::init(argc, argv, "get_weight_server");
    ros::NodeHandle n;

    ros::ServiceServer service = n.advertiseService("get_weight", getWeights);
    
    ROS_INFO("Ready to DMP.");
    ros::spin();
    return 0;
}
