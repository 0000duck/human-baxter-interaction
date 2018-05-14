#include <iostream>
#include "ros/ros.h"
#include "dmp/integration.h"
#include "dmp/function.h"
#include "dmp/getWeight.h"
#include "dmp/dmp.h"
#include "geometry_msgs/Pose.h"
#include <std_msgs/Float32MultiArray.h>
#include <tf/transform_broadcaster.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;


bool getIntegration(dmp::integration::Request &req, dmp::integration::Response &res)
{
    res.goal = req.test.writeFile;
    float time = req.test.duration;
    string readPath = req.test.readFile;
    string writePath = req.test.writeFile;
    VectorXd t_ini = VectorXd::LinSpaced(200, 0, time);
    VectorXd t = t_ini / time;
    return true;
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "integration_server");
    ros::NodeHandle n;
    std_msgs::Float32MultiArray aa;
    aa.data = {1,2,3,4,5,6,7,8};
    cout<<"the length og array:"<<aa.data.size()<<endl;
    for(int i=0; i!=8; ++i)
        cout<<"\n///////////////"<<aa.data[i]<<"\n@@@@@@@@@"<<endl;

    ros::ServiceServer service = n.advertiseService("get_integration", getIntegration);
/////////////////////////////////////////////////////////////// 

    tf::Quaternion q1,q2,q3;

    Vector3d xx(1,2,3),yy;

    q1.setRPY(0, 0, 1);

    Quaternion<double,AutoAlign> a(q1);
    cout<<a.coeffs()<<endl;
    cout<<"\n//////////////////"<<endl;
    for(int i=0; i!=4; ++i)
    {
        cout<<q1[i]<<endl;
    }  

    cout<<"\n//////////////////"<<endl;
    yy = a*xx;
    cout<<yy<<endl;

///////////////////////////////////////////////////////////////
    ROS_INFO("Ready to DMP.");
    ros::spin();
    return 0;
}
