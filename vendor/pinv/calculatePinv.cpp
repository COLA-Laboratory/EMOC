//
// Created by ning on 11/9/20.
//

using namespace std;
#include <iostream>
#include <stdlib.h>
#include "calculatePinv.h"
#include "eigen-3.3.8/Eigen/SVD"

Eigen::MatrixXd pinv_eigen_based(Eigen::MatrixXd & origin, const float er = 0)
{
    // 进行svd分解
    Eigen::JacobiSVD<Eigen::MatrixXd> svd_holder(origin,
                                                 Eigen::ComputeThinU |
                                                 Eigen::ComputeThinV);
    // 构建SVD分解结果
    Eigen::MatrixXd U = svd_holder.matrixU();
    Eigen::MatrixXd V = svd_holder.matrixV();
    Eigen::MatrixXd D = svd_holder.singularValues();

    // 构建S矩阵
    Eigen::MatrixXd S(V.cols(), U.cols());
    S.setZero();

    for (unsigned int i = 0; i < D.size(); ++i) {

        if (D(i, 0) > er) {
            S(i, i) = 1 / D(i, 0);
        }
        else {
            S(i, i) = 0;
        }
    }

    // pinv_matrix = V * S * U^T
    return V * S * U.transpose();
}

/*calculate pinv | new zone | hold old zone on*/
double** calculatePinv(double** n, int row, int col)
{
    Eigen::MatrixXd originMatrix(row,col);
    Eigen::MatrixXd pinvMatrix;
    double** pinvN;
    pinvN = (double **)malloc(col*sizeof(double *));

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            originMatrix(i,j) = n[i][j];
        }
    }
    pinvMatrix = pinv_eigen_based(originMatrix);
    for (int i = 0; i < col; ++i)
    {
        pinvN[i] = (double *)malloc(row*sizeof(double));
        for (int j = 0; j < row; ++j)
        {
            pinvN[i][j] = pinvMatrix(i,j);
        }
    }
    return pinvN;
}

