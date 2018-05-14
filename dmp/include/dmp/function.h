#ifndef FUNCTION_H
#define FUNCTION_H
#include "dmp/dmp.h"
#include <vector>
#include <fstream>
#include <boost/numeric/odeint.hpp>
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
using namespace Eigen;
using namespace std;
typedef std::vector< double > state_type;

void RotationMatrix(VectorXd oldIni, VectorXd oldGoal, 
                        VectorXd newIni, VectorXd newGoal, VectorXd& dy );

MatrixXd readTxt(string path);
void writeTxt(string path, MatrixXd demo);



void numDiff1(Eigen::MatrixXd &dX, const Eigen::MatrixXd &X, const Eigen::VectorXd &t);
void numDiff2(Eigen::MatrixXd &dX, Eigen::MatrixXd &d2X, const Eigen::MatrixXd &X, const Eigen::VectorXd &t);
void numDiff1(Eigen::MatrixXd &dX, const Eigen::MatrixXd &X, double dt);
void numDiff2(Eigen::MatrixXd &dX, Eigen::MatrixXd &d2X, const Eigen::MatrixXd &X, double dt);


double force(double t, double alphaX, int numKernel, double goal, double start, VectorXd c, VectorXd h, VectorXd w);


struct push_back_state_and_time
{
    std::vector< state_type >& m_states;
    std::vector< double >& m_times;

    push_back_state_and_time( std::vector< state_type > &states , std::vector< double > &times )
    : m_states( states ) , m_times( times ) { }

    void operator()( const state_type &x , double t )
    {
        m_states.push_back( x );
        m_times.push_back( t );
    }
};

class damp_sys {

    int numKernel;
    int alphaX;//指数函数衰减率
    double alpha;
    double beta;
    double goal;
    double start;
    double tau;
    VectorXd c;
    VectorXd h;
    VectorXd w;

public:
    damp_sys( int p_alphaX, int p_numKernel, double p_alpha, double p_beta, double p_goal, double p_start,double p_tau,
              VectorXd p_c, VectorXd p_h, VectorXd p_w ) :
              alphaX(p_alphaX), numKernel(p_numKernel), alpha(p_alpha), beta(p_beta), goal(p_goal), start(p_start),
              tau(p_tau), c(p_c), h(p_h), w(p_w) {}

    void operator() ( const state_type &x , state_type &dxdt , const double t )
    {
        dxdt[0] = x[1];
        dxdt[1] = (alpha*(beta*(goal - x[0]) -x[1]) + force(t, alphaX, numKernel,goal, start, c, h, w)) / tau;
    }
};


#endif // FUNCTION_H
