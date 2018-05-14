#include <iostream>
#include "ros/ros.h"
#include "dmp/function.h"
#include "dmp/getTrajectory.h"
#include "dmp/dmp.h"

using namespace std;


bool getTrajectorys(dmp::getTrajectory::Request &req, dmp::getTrajectory::Response &res)
{
    res.trajectory = req.inputData.writeFile;
    float time = req.inputData.duration;
    string readPath = req.inputData.readFile;
    string writePath = req.inputData.writeFile;

    float sdx = req.inputData.s_dx;
    float sdy = req.inputData.s_dy;
    float sdz = req.inputData.s_dz;

    float edx = req.inputData.e_dx;
    float edy = req.inputData.e_dy;
    float edz = req.inputData.e_dz;

    VectorXd t_ini = VectorXd::LinSpaced(200, 0, time);
    VectorXd t = t_ini / time;

    int numKernel = 30;
    int alphaX = 1;
    double tau = 1;
    double alpha = 25;
    DMP obj(numKernel, alphaX, tau, alpha );

    MatrixXd w_t = readTxt(readPath);
    MatrixXd w = w_t.topRows(w_t.rows()-3);
    VectorXd oldIni = w_t.row(w_t.rows()-3);
    VectorXd oldGoal = w_t.row(w_t.rows()-2);
    VectorXd dy = w_t.row(w_t.rows()-1);
    VectorXd newGoal = oldGoal;
    VectorXd newIni = oldIni;
    
    
    cout<<newIni<<endl;
    cout<<endl;
    cout<<newGoal<<endl;
    cout<<endl;
    newIni(0)+=sdx;
    newIni(1)+=sdy;
    newIni(2)+=sdz;

    cout<<sdx<<" "<<sdy<<" "<<sdz<<endl;
    cout<<edx<<" "<<edy<<" "<<edz<<endl<<endl;

    newGoal(0)+=edx;
    newGoal(1)+=edy;
    newGoal(2)+=edz;

    cout<<newIni<<endl;
    cout<<endl;
    cout<<newGoal<<endl;

    RotationMatrix(oldIni, oldGoal, newIni, newGoal, dy);

    int D = w_t.cols();
    VectorXd c,h;
    for(int i=0; i!=D; ++i)
        VectorXd* psi = obj.getPsi(t, c, h);

    MatrixXd trajectory;
    state_type x(2);
    for(int i=0; i!=D; ++i)
    {
        x[0] = newIni(i);
        x[1] = dy(i);
        VectorXd test = obj.getTrajectory(t.rows()-1, newGoal(i), newIni(i), x, c, h, w.col(i));
        trajectory.resize(test.rows(),D+1);
        trajectory.col(i+1) = test;
    }
    for(int i=0; i!=trajectory.rows(); ++i)
    {
        double a = i*t_ini(t_ini.rows()-1);
        double b = t.rows()-1;
        trajectory(i,0) = a/b;
    }
    writeTxt(writePath, trajectory);
    cout<<"\nThe trajectory has already save to: "<<res.trajectory<<"\n"<<endl;
    return true;
}


int main(int argc, char *argv[])
{
    ros::init(argc, argv, "get_trajectory_server");
    ros::NodeHandle n;
    ros::ServiceServer service = n.advertiseService("get_trajectory", getTrajectorys);
    ROS_INFO("Ready to create trajectory.");
    ros::spin();
    return 0;
}
