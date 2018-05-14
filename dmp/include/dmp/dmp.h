#ifndef DMP_H
#define DMP_H

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cmath>
#include "dmp/function.h"
#include <boost/numeric/odeint.hpp>

using namespace Eigen;
using namespace boost::numeric::odeint;
using namespace std;
typedef std::vector< double > state_type;

class DMP
{
private:
    int numKernel;
    int alphaX;//指数函数衰减率
    double tau;//调节轨迹速度
    double alpha;
public:
    DMP(int num, int al, double ta, double alp): numKernel(num), alphaX(al), tau(ta), alpha(alp){}
    
    int getNumKernel();
    VectorXd fTarget(VectorXd dy, VectorXd ddy, const VectorXd y);
    VectorXd* getPsi(const VectorXd t, VectorXd &c, VectorXd &h);
    VectorXd weight(double goal, double start, VectorXd *psi, VectorXd t, VectorXd fTarget);
    void saveWeight(string readPath, string writePath);
    VectorXd getTrajectory(double time_end, double goal, double start, state_type x, VectorXd c, VectorXd h, VectorXd w);

};

#endif // DMP_H
