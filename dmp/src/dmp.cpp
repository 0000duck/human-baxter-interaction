#include "dmp/dmp.h"

int DMP::getNumKernel()
{
    return numKernel;
}

VectorXd DMP::fTarget(VectorXd dy, VectorXd ddy, const VectorXd y)
{
    double beta = alpha/4;
    VectorXd dis = y(y.rows()-1) - y.array();
    int tau = 1;
    VectorXd target = ddy*tau*tau - alpha*( beta*dis - tau*dy );
    return target;
}

VectorXd *DMP::getPsi(const VectorXd t, VectorXd &c, VectorXd &h)
{
    int end = t.rows() - 1;
    double first = exp(-1 * alphaX * t(end));
    double last = t(end)*1.1 - first;
    c = VectorXd::LinSpaced(numKernel,first,last);
    c = -c.array().log();
    VectorXd x = (-alphaX * t).array().exp();

    h = pow(numKernel,1.5) * c.array().inverse() * 16;
    VectorXd *psi = new VectorXd[numKernel];
    for(int i=0; i!=numKernel; ++i)
        psi[i] = ( (-1)*h(i) * (x.array()-c(i)).array().square()  ).array().exp();
    return psi;
}

VectorXd DMP::weight(double goal, double start, VectorXd *psi, VectorXd t, VectorXd fTarget)
{

    MatrixXd *gamma = new MatrixXd[numKernel];
    for(int i=0; i!=numKernel; ++i)
        gamma[i] = (psi[i]).asDiagonal();

    VectorXd x = (-alphaX * t).array().exp();
    VectorXd ksi = x*(goal - start);

    VectorXd w(numKernel);
    for(int i=0; i!=numKernel; ++i)
    {
        double aa = ksi.adjoint() * gamma[i] * fTarget ;
        double bb = ksi.adjoint() * gamma[i] * ksi;
        w(i) = aa / bb;
    }
    return w;
}
void DMP::saveWeight(string readPath, string writePath)
{
    MatrixXd demo = readTxt(readPath);
    int D = demo.cols()-1;//轨迹维数 不算时间
    VectorXd goal = demo.bottomRightCorner(1,D).adjoint();
    VectorXd start = demo.topRightCorner(1,D).adjoint();
    MatrixXd dy_t, ddy_t, demo_t;
    VectorXd c,h;
    demo_t = demo.rightCols(D);
    VectorXd t_ini = demo.col(0);
    VectorXd t = t_ini/t_ini(t_ini.rows()-1);
    numDiff2(dy_t, ddy_t, demo_t.adjoint(), t);
    MatrixXd dy = dy_t.adjoint();
    MatrixXd ddy = ddy_t.adjoint();

    MatrixXd w(numKernel, D);
    for(int i=0; i!=D; ++i)
    {
        VectorXd y = demo_t.col(i);
        VectorXd f_t = fTarget(dy.col(i), ddy.col(i), y);
        VectorXd* psi = getPsi(t, c, h);
        w.col(i) = weight(goal(i), start(i), psi, t, f_t);
    }
    MatrixXd saveW(numKernel+3, D);
    saveW.topRows(numKernel) = w;
    saveW.row(numKernel) = start;
    saveW.row(numKernel+1) = goal;
    saveW.row(numKernel+2) = dy.row(0);

    writeTxt(writePath, saveW);
}


VectorXd DMP::getTrajectory(double time_end, double goal, double start, state_type x, VectorXd c, VectorXd h, VectorXd w)//x为初始状态
{

    double beta = alpha/4;
    damp_sys dam(alphaX, numKernel, alpha, beta, goal, start,tau, c, h, w);
    vector<state_type> x_vec;
    vector<double> times;
    runge_kutta4< state_type > stepper;
    size_t steps = integrate_const( stepper , dam , x , 0.0 , 1.0 , 1.0/time_end, push_back_state_and_time( x_vec , times ) );
    VectorXd trajectory(steps+1);
    for(size_t i=0; i<=steps; i++ )
    {
        trajectory(i) = x_vec[i][0];
    }
    return trajectory;
}
