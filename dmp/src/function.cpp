#include "dmp/function.h"
// #include "dmp/dmp.h"

void RotationMatrix(VectorXd oldIni, VectorXd oldGoal, 
                        VectorXd newIni, VectorXd newGoal, VectorXd& dy )
{
    VectorXd oldVec_t = oldGoal - oldIni;
    Vector3d oldVector = oldVec_t.topRows(3);
    VectorXd newVec_t = newGoal - newIni;
    Vector3d newVector = newVec_t.topRows(3);
    Quaternion<double,AutoAlign> rotationQua;
    rotationQua = rotationQua.FromTwoVectors(oldVector,newVector);
    Vector3d position = dy.topRows(3);
    position = rotationQua * position;
    Vector4d qua = dy.bottomRows(4);
    Quaternion<double,AutoAlign> oldQua(qua(3), qua(0), qua(1), qua(2));
    Quaternion<double,AutoAlign> newQua = rotationQua * oldQua;
    qua = newQua.coeffs();
    dy.topRows(3) = position; 
    dy.bottomRows(4) = qua;
}


MatrixXd readTxt(string path)
{
    int dimension = 1;
    ifstream infile(path, ios::in);
    int m_i=2;
    string buff, n ;

    getline(infile,buff);
    getline(infile,buff);
    for(int i=0; i!=buff.length(); ++i)
        if(buff[i] == ',')
            ++dimension;  
    while (getline(infile,buff))
        ++m_i;
    infile.close();
    infile.open(path, ios::in);
    MatrixXd mat(m_i-1,dimension);
    int flag=0;
    while (getline(infile,buff))
    {
        if(flag++ ==0)
            continue;
        for(int i=0; i!=dimension; ++i)
        {
            n = buff.substr(0,buff.find_first_of(',') );
            buff.erase(0,n.size()+1);
            char *p = const_cast<char*>(n.c_str());
            double num = atof(p);
            mat(flag-2,i) = num;
        }
    }
    return mat;
}
void writeTxt(string path, MatrixXd demo)
{
    ofstream out;
    string str= "time,left_s0,left_s1,left_e0,left_e1,left_w0,left_w1,left_w2,right_s0,right_s1,right_e0,right_e1,right_w0,right_w1,right_w2,";
    out.open(path, ios::out);
    out<<str<<endl;
    for(int i=0; i!=demo.rows(); ++i)
    {
        for(int j=0; j!=demo.cols()-1; ++j)
            out<<demo(i,j)<<',';
        out<<demo(i,demo.cols()-1)<<endl;
    }
}




void numDiff1(Eigen::MatrixXd &dX, const Eigen::MatrixXd &X, const VectorXd &t)
{
    // f0   [ 1, (t0-t1), (t0-t1)^2/2 ]   f1
    // f1 = [ 1,       0,           0 ] x f1'
    // f2   [ 1, (t2-t1), (t2-t1)^2/2 ]   f1''

    int N = X.cols();

    dX = MatrixXd::Zero(X.rows(), N);

    if (N <= 1){
        return;
    } else if (N == 2){
        dX.col(0) = dX.col(1) = (X.col(1) - X.col(0)) / (t(1) - t(0));
        return;
    }

    VectorXd N1(3);
    double a0, a2;

    a0 = t(1)-t(0); a2 = t(2)-t(0);
    N1 << -(a0+a2)/(a0*a2), -a2/(a0*(a0-a2)), a0/(a2*(a0-a2));
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(0) = X.middleCols(0, 3) * N1;

    for (int i = 2 ; i < N-2 ; i++){
        a0 = t(i-1)-t(i); a2 = t(i+1)-t(i);
        N1 << -a2/(a0*(a0-a2)), -(a0+a2)/(a0*a2), a0/(a2*(a0-a2));
        // we only need f' and f'' so there is no need to calculate full matrix
        dX.col(i) = X.middleCols(i-1, 3) * N1;
    }

    a0 = t(N-3)-t(N-1); a2 = t(N-2)-t(N-1);
    N1 << -a2/(a0*(a0-a2)), a0/(a2*(a0-a2)), -(a0+a2)/(a0*a2);
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(N-1) = X.middleCols(N-3, 3) * N1;

}

void numDiff2(Eigen::MatrixXd &dX, Eigen::MatrixXd &d2X, const Eigen::MatrixXd &X, const Eigen::VectorXd &t)
{
    // f0   [ 1, (t0-t2), (t0-t2)^2/2, (t0-t2)^3/6, (t0-t2)^4/24 ]   f2
    // f1   [ 1, (t1-t2), (t1-t2)^2/2, (t1-t2)^3/6, (t1-t2)^4/24 ]   f2'
    // f2 = [ 1,       0,           0,           0,            0 ] x f2''
    // f3   [ 1, (t3-t2), (t3-t2)^2/2, (t3-t2)^3/6, (t3-t2)^4/24 ]   f2(3)
    // f4   [ 1, (t4-t2), (t4-t2)^2/2, (t4-t2)^3/6, (t4-t2)^4/24 ]   f2(4)

    int N = X.cols();

    dX = MatrixXd::Zero(X.rows(), N);
    d2X = MatrixXd::Zero(X.rows(), N);

    if (N < 5){
        numDiff1(dX, X, t);
        return;
    }


    VectorXd N1(5), N2(5), D(5);
    double a0, a1, a3, a4;

    a0 = t(1)-t(0); a1 = t(2)-t(0); a3 = t(3)-t(0); a4 = t(4)-t(0);
    N1 << -a0*a1*a3-a0*a1*a4-a0*a3*a4-a1*a3*a4, a1*a3*a4, a0*a3*a4, a0*a1*a4, a0*a1*a3;
    N2 << -a0*a1-a0*a3-a0*a4-a1*a3-a1*a4-a3*a4, a1*a3+a1*a4+a3*a4, a0*a3+a0*a4+a3*a4, a0*a1+a0*a4+a1*a4, a0*a1+a0*a3+a1*a3;
    N2 *= -2.0f;
    D << a0*a1*a3*a4, -a0*(a0-a1)*(a0-a3)*(a0-a4), a1*(a0-a1)*(a1-a3)*(a1-a4), -a3*(a0-a3)*(a1-a3)*(a3-a4), a4*(a0-a4)*(a1-a4)*(a3-a4);
    N1 = N1.cwiseQuotient(D);
        N2 = N2.cwiseQuotient(D); //逐元素除法
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(0) = X.middleCols(0, 5) * N1;
    d2X.col(0) = X.middleCols(0, 5) * N2;

    a0 = t(0)-t(1); a1 = t(2)-t(1); a3 = t(3)-t(1); a4 = t(4)-t(1);
    N1 << a1*a3*a4, -a0*a1*a3-a0*a1*a4-a0*a3*a4-a1*a3*a4, a0*a3*a4, a0*a1*a4, a0*a1*a3;
    N2 << a1*a3+a1*a4+a3*a4, -a0*a1-a0*a3-a0*a4-a1*a3-a1*a4-a3*a4, a0*a3+a0*a4+a3*a4, a0*a1+a0*a4+a1*a4, a0*a1+a0*a3+a1*a3;
    N2 *= -2.0f;
    D << -a0*(a0-a1)*(a0-a3)*(a0-a4), a0*a1*a3*a4, a1*(a0-a1)*(a1-a3)*(a1-a4), -a3*(a0-a3)*(a1-a3)*(a3-a4), a4*(a0-a4)*(a1-a4)*(a3-a4);
    N1 = N1.cwiseQuotient(D);
    N2 = N2.cwiseQuotient(D);
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(1) = X.middleCols(0, 5) * N1;
    d2X.col(1) = X.middleCols(0, 5) * N2;

    for (int i = 2 ; i < N-2 ; i++){
        a0 = t(i-2)-t(i); a1 = t(i-1)-t(i); a3 = t(i+1)-t(i); a4 = t(i+2)-t(i);
        N1 << a1*a3*a4, a0*a3*a4, -a0*a1*a3-a0*a1*a4-a0*a3*a4-a1*a3*a4, a0*a1*a4, a0*a1*a3;
        N2 << a1*a3+a1*a4+a3*a4, a0*a3+a0*a4+a3*a4, -a0*a1-a0*a3-a0*a4-a1*a3-a1*a4-a3*a4, a0*a1+a0*a4+a1*a4, a0*a1+a0*a3+a1*a3;
        N2 *= -2.0f;
        D << -a0*(a0-a1)*(a0-a3)*(a0-a4), a1*(a0-a1)*(a1-a3)*(a1-a4), a0*a1*a3*a4, -a3*(a0-a3)*(a1-a3)*(a3-a4), a4*(a0-a4)*(a1-a4)*(a3-a4);
        N1 = N1.cwiseQuotient(D);
        N2 = N2.cwiseQuotient(D);
        // we only need f' and f'' so there is no need to calculate full matrix
        dX.col(i) = X.middleCols(i-2, 5) * N1;
        d2X.col(i) = X.middleCols(i-2, 5) * N2;
    }

    a0 = t(N-5)-t(N-2); a1 = t(N-4)-t(N-2); a3 = t(N-3)-t(N-2); a4 = t(N-1)-t(N-2);
    N1 << a1*a3*a4, a0*a3*a4, a0*a1*a4, -a0*a1*a3-a0*a1*a4-a0*a3*a4-a1*a3*a4, a0*a1*a3;
    N2 << a1*a3+a1*a4+a3*a4, a0*a3+a0*a4+a3*a4, a0*a1+a0*a4+a1*a4, -a0*a1-a0*a3-a0*a4-a1*a3-a1*a4-a3*a4, a0*a1+a0*a3+a1*a3;
    N2 *= -2.0f;
    D << -a0*(a0-a1)*(a0-a3)*(a0-a4), a1*(a0-a1)*(a1-a3)*(a1-a4), -a3*(a0-a3)*(a1-a3)*(a3-a4), a0*a1*a3*a4, a4*(a0-a4)*(a1-a4)*(a3-a4);
    N1 = N1.cwiseQuotient(D);
    N2 = N2.cwiseQuotient(D);
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(N-2) = X.middleCols(N-5, 5) * N1;
    d2X.col(N-2) = X.middleCols(N-5, 5) * N2;

    a0 = t(N-5)-t(N-1); a1 = t(N-4)-t(N-1); a3 = t(N-3)-t(N-1); a4 = t(N-2)-t(N-1);
    N1 << a1*a3*a4, a0*a3*a4, a0*a1*a4, a0*a1*a3, -a0*a1*a3-a0*a1*a4-a0*a3*a4-a1*a3*a4;
    N2 << a1*a3+a1*a4+a3*a4, a0*a3+a0*a4+a3*a4, a0*a1+a0*a4+a1*a4, a0*a1+a0*a3+a1*a3, -a0*a1-a0*a3-a0*a4-a1*a3-a1*a4-a3*a4;
    N2 *= -2.0f;
    D << -a0*(a0-a1)*(a0-a3)*(a0-a4), a1*(a0-a1)*(a1-a3)*(a1-a4), -a3*(a0-a3)*(a1-a3)*(a3-a4), a4*(a0-a4)*(a1-a4)*(a3-a4), a0*a1*a3*a4;
    N1 = N1.cwiseQuotient(D);
    N2 = N2.cwiseQuotient(D);
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(N-1) = X.middleCols(N-5, 5) * N1;
    d2X.col(N-1) = X.middleCols(N-5, 5) * N2;
}

void numDiff1(Eigen::MatrixXd &dX, const Eigen::MatrixXd &X, double dt)
{
    // f0   [ 1, -dt, dt^2/2 ]   f1
    // f1 = [ 1,   0,      0 ] x f1'
    // f2   [ 1,  dt, dt^2/2 ]   f1''

    int N = X.cols();

    dX = MatrixXd::Zero(X.rows(), N);

    if (N <= 1){
        return;
    } else if (N == 2){
        dX.col(0) = dX.col(1) = (X.col(1) - X.col(0)) / dt;
        return;
    }

    VectorXd N1(3);

    N1 << -1.5f/dt, 2.0f/dt, -0.5f/dt;
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(0) = X.middleCols(0, 3) * N1;

    N1 << -0.5f/dt, 0.0f, 0.5f/dt;
    for (int i = 2 ; i < N-2; i++){
        // we only need f' and f'' so there is no need to calculate full matrix
        dX.col(i) = X.middleCols(i-1, 3) * N1;
    }

    N1 << 0.5f/dt, -2.0f/dt, 1.5f/dt;
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(N-1) = X.middleCols(N-3, 3) * N1;
}

void numDiff2(Eigen::MatrixXd &dX, Eigen::MatrixXd &d2X, const Eigen::MatrixXd &X, double dt)
{
    // f0   [ 1, -2*dt, dt^2*2, -4*dt^3/3, 2*dt^4/3 ]   f2
    // f1   [ 1,   -dt, dt^2/2,   -dt^3/6,  dt^4/24 ]   f2'
    // f2 = [ 1,     0,      0,         0,        0 ] x f2''
    // f3   [ 1,    dt, dt^2/2,    dt^3/6,  dt^4/24 ]   f2(3)
    // f4   [ 1,  2*dt, dt^2*2,  4*dt^3/3, 2*dt^4/3 ]   f2(4)
    int N = X.cols();

    if (N < 5){
        numDiff1(dX, X, dt);
        return;
    }

    dX = MatrixXd::Zero(X.rows(), N);
    d2X = MatrixXd::Zero(X.rows(), N);

    VectorXd N1(5), N2(5);
    double dt2 = dt*dt;

    N1 << -25.0f/12.0f/dt, 4.0f/dt, -3.0f/dt, 4.0f/3.0f/dt, -0.25f/dt;
    N2 << 35.0f/12.0f/dt2, -26.0f/3.0f/dt2, 9.5f/dt2, -14.0f/3.0f/dt2, 11.0f/12.0f/dt2;
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(0) = X.middleCols(0, 5) * N1;
    d2X.col(0) = X.middleCols(0, 5) * N2;

    N1 << -0.25f/dt, -5.0f/6.0f/dt, 1.5f/dt, -0.5f/dt, 1.0f/12.0f/dt;
    N2 << 11.0f/12.0f/dt2, -5.0f/3.0f/dt2, 0.5f/dt2, 1.0f/3.0f/dt2, -1.0f/12.0f/dt2;
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(1) = X.middleCols(0, 5) * N1;
    d2X.col(1) = X.middleCols(0, 5) * N2;

    N1 << 1.0f/12.0f/dt, -2.0f/3.0f/dt, 0.0f, 2.0f/3.0f/dt, -1.0f/12.0f/dt;
    N2 << -1.0f/12.0f/dt2, 4.0f/3.0f/dt2, -2.5f/dt2, 4.0f/3.0f/dt2, -1.0f/12.0f/dt2;
    for (int i = 2 ; i < N-2; i++){
        // we only need f' and f'' so there is no need to calculate full matrix
        dX.col(i) = X.middleCols(i-2, 5) * N1;
        d2X.col(i) = X.middleCols(i-2, 5) * N2;
    }

    N1 << -1.0f/12.0f/dt, 0.5f/dt, -1.5f/dt, 5.0f/6.0f/dt, 0.25f/dt;
    N2 << -1.0f/12.0f/dt2, 1.0f/3.0f/dt2, 0.5f/dt2, -5.0f/3.0f/dt2, 11.0f/12.0f/dt2;
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(N-2) = X.middleCols(N-5, 5) * N1;
    d2X.col(N-2) = X.middleCols(N-5, 5) * N2;

    N1 << 0.25f/dt, -4.0f/3.0f/dt, 3.0f/dt, -4.0f/dt, 25.0f/12.0f/dt;
    N2 << 11.0f/12.0f/dt2, -14.0f/3.0f/dt2, 9.5f/dt2, -26.0f/3.0f/dt2, 35.0f/12.0f/dt2;
    // we only need f' and f'' so there is no need to calculate full matrix
    dX.col(N-1) = X.middleCols(N-5, 5) * N1;
    d2X.col(N-1) = X.middleCols(N-5, 5) * N2;
}

double force(double t, double alphaX, int numKernel, double goal, double start, VectorXd c, VectorXd h, VectorXd w)
{
    double x = exp(-alphaX * t);
    double ksi = x*(goal - start);
    double sumDown = 0;
    double sumUp = 0;
    for(int i=0; i!=numKernel; ++i)
    {
        sumDown = sumDown + exp( (-1)*h(i) * pow( (x-c(i)),2 ) );
        sumUp = sumUp + exp( (-1)*h(i) * pow( (x-c(i)),2 ) ) * w(i);
    }
    double f = (sumUp / sumDown) * ksi;
    return f;
}

