#include <ArduinoEigen.h>
#include <ArduinoEigenDense.h>
#include <ArduinoEigenSparse.h>

using namespace Eigen;

RowVector<float,10> fit_ellipsoid(MatrixXf samples)
{
    int N = samples.cols();
    // Design matrix
    VectorXf x = samples.row(0);
    VectorXf y = samples.row(1);
    VectorXf z = samples.row(2);

    // Create design matrix
    MatrixXf D = MatrixXf::Zero(N,10);

    D << x.array().pow(2), y.array().pow(2), z.array().pow(2), 2*y.array()*z.array(), 2*x.array()*z.array(), 2*x.array()*y.array(), 2*x.array(), 2*y.array(), 2*z.array(), VectorXf::Ones(N);
    D.transposeInPlace();

    // D seems correct
    //std::cout << "D: \n" << D << "\n\n";


    // Apply constraint kJ > I^2
    // The quadratic surface is an ellipse if k = 4
    int k = 4;

    // Create constrain matrix C - Eq(7)
    Matrix<float,6,6> C;
    C <<   -1, 0.5*k-1, 0.5*k-1, 0, 0, 0,
            0.5*k-1, -1, 0.5*k-1, 0, 0, 0,
            0.5*k-1, 0.5*k-1, -1, 0, 0, 0,
            0, 0, 0, -k, 0, 0,
            0, 0, 0, 0, -k, 0,
            0, 0, 0, 0, 0, -k;

    // Create S matrix from D.T*D - Eqn(11)
    MatrixXf S = D * D.transpose();
    Matrix<float,6,6> S11 = S.block<6,6>(0,0);
    Matrix<float,6,4> S12 = S.block<6,4>(0,6);
    Matrix<float,4,6> S21 = S.block<4,6>(6,0);
    Matrix<float,4,4> S22 = S.block<4,4>(6,6);

//    std::cout << "\n\n";
//    std::cout << "S11: \n" << S11 << "\n\n";
//    std::cout << "S12: \n" << S12 << "\n\n";
//    std::cout << "S21: \n" << S21 << "\n\n";
//    std::cout << "S22: \n" << S22 << "\n\n";


    // Solve least squares - Eqn(14) and Eqn(15)
    MatrixXf M  = C.inverse() * (S11 - S12*S22.inverse() * S21);
    EigenSolver<MatrixXf> es(M);
    Vector<std::complex<float>,6> eigenvalues = es.eigenvalues();
    Matrix<std::complex<float>,6,6> eigenvectors = es.eigenvectors();

    Vector<float,6> eval = eigenvalues.array().real();
    Matrix<float,6,6> evec = eigenvectors.array().real();
//    std::cout << "Matrix to decompose: \n" << M << "\n";
//    std::cout << "Eigenvalues: \n" << eval << "\n";
//    std::cout << "Eigenvectors: \n" << evec << "\n\n";

    // Find eigenvector corresponding to largest eigenvalue
    Vector<float,6> u1;
    for (int i=0;i<6;i++)
    {
        if(eval[i] > 0.0) {
            u1 = evec.col(i);
            break;
        } else if (i == 5) {
            throw std::runtime_error("No positive eigenvalues found!");
        }
    }

    Vector<float,4> u2 = (-(S22.inverse() * S21) * u1);
    Vector<float,10> U;
    U << u1, u2;
    return U;
}

Vector<float,12> calculate_ellipsoid_transformation(Matrix3f M, Vector3f n, float d)
{
    // Inverse of M
    Matrix3f M_ = M.inverse();
    // Calculate offset vector
    Vector3f b = -M_ * n;

    /******************************************************************************************
     * Equation (17) of https://teslabs.com/articles/magnetometer-calibration/
     * Used to convert ellipsoid parameters to a transformation matrix back to a unit sphere
     * Matrix square root follows https://math.stackexchange.com/a/59391
     ******************************************************************************************/
    EigenSolver<MatrixXf> es(M);
    Matrix3cf eval = es.eigenvalues().asDiagonal();
    Matrix3cf evec = es.eigenvectors();

    // Calculate diagonal matrix of square roots of eigenvectors
    Matrix3cf eval_sqrt;
    eval_sqrt << eval.array().sqrt();

    // Calculate square root of matrix
    Matrix3cf M_sqrt;
    M_sqrt << evec * eval_sqrt * evec.inverse();

    // Calculate inner square root
    std::complex<float> inner = n.transpose() * M_ * n - d;
    std::complex<float> sqrtinv = (std::complex<float>)pow(sqrt(inner),-1);

    // Calculate final transformation matrix
    Matrix3cf A_1;
    A_1 << (sqrtinv * M_sqrt).array();
    Matrix3f A_1real = A_1.array().real();

    // Return as a vector
    Map<VectorXf> v1(A_1real.data(),9);
    Map<VectorXf> v2(b.data(), 3);
    Vector<float,12> V;
    V << v1, v2;
    //A_1 << A_1.inverse();
    // std::cout << "Transformation matrix: \n" << v1.reshaped(3,3) << "\n\n";
    // std::cout << "Bias vector: \n" << v2.reshaped(3,1) << "\n\n";
    return V;
}
