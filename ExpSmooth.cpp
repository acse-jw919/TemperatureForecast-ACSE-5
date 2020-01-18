#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>

#define MAXPRE 100

using namespace std;

class ExponentialSmoothing{
public:
    static double* basicES(vector<double> data, int length);
    static double* doubleES(vector<double> data, int length);
    static double* tripleES(vector<double> data, int length);
private:
    static double* optimize(vector<double> data, int nparam, double* (*stepFunc)(vector<double>, int, double&, double*));
    static double* basicStep(vector<double> data, int, double &rmse, double *param);
    static double* doubleStep(vector<double> data, int, double &rmse, double *param);
    static double* tripleStep(vector<double> data, int, double &rmse, double *param);
};

double* ExponentialSmoothing::basicES(vector<double> data, int length){
    double* param = optimize(data, 1, basicStep);
    double rmse;
    double* predicted = basicStep(data, length, rmse, param);
    return predicted;
}

double* ExponentialSmoothing::doubleES(vector<double> data, int length){
    double* param = optimize(data, 2, doubleStep);
    double rmse;
    double* predicted = doubleStep(data, length, rmse, param);
    return predicted;
}

double* ExponentialSmoothing::tripleES(vector<double> data, int length){
    double* param = optimize(data, 3, tripleStep);
    double rmse;
    double* predicted = tripleStep(data, length, rmse, param);
    return predicted;
}

double* ExponentialSmoothing::optimize(vector<double> data, int nparam, double* (*stepFunc)(vector<double>, int, double&, double*)){
    double* param = new double[3];
    double* best_param = new double[3];
    double best_rmse = 10000;
    for(int i = 1; i < MAXPRE; i++){
        param[0] = double(i) / MAXPRE;
        for(int j = 1; j < (nparam >= 2 ? MAXPRE : 2); j++){
            param[1] = double(j) / MAXPRE;
            for(int k = 1; k < (nparam >= 3 ? MAXPRE : 2); k++){
                param[2] = double(k) / MAXPRE;
                double rmse = 0.0;
                double* predicted = stepFunc(data, 1, rmse, param);
                rmse = sqrt(rmse / data.size());
                if(rmse < best_rmse){
                    for(int np = 0; np < nparam; np++){
                        best_param[np] = param[np];
                    }
                    best_rmse = rmse;
                }
            }
        }
    }
    cout << "RMSE: " << best_rmse;
    for(int i = 0; i < nparam; i++){
        cout << " " << best_param[i];
    }
    cout << endl;
    return best_param;
}

double* ExponentialSmoothing::basicStep(vector<double> data, int length, double &rmse, double *param){
    double s = data.at(0);
    for(int i = 1; i < data.size(); i++){
        rmse += pow(s - data.at(i), 2);
        s = param[0] * data.at(i) + (1 - param[0]) * s;
    }
    double* predicted = new double[1];
    predicted[0] = s;
    return predicted;
}

double* ExponentialSmoothing::doubleStep(vector<double> data, int length, double &rmse, double *param){
    double s = data.at(1);
    double b = data.at(1) - data.at(0);
    for(int i = 2; i < data.size(); i++){
        rmse += pow(s + b - data.at(i), 2);
        double s_prev = s;
        s = param[0] * data.at(i) + (1 - param[0]) * (s + b);
        b = param[1] * (s - s_prev) + (1 - param[1]) * b;
    }
    double* predicted = new double[length];
    predicted[0] = s + b;
    for(int i = 1; i < length; i++){
        predicted[i] = s + (i + 1) * b;
    }
    return predicted;
}

double* ExponentialSmoothing::tripleStep(vector<double> data, int length, double &rmse, double *param){
    int L = 12;
    double s = data.at(L);
    double b = 0.0;
    static vector<double> c;
    int N = (int) (data.size() / L);
    for(int i = 0; i < L; i++){
        b += (data.at(L + i) - data.at(i));
    }
    b /= pow(L, 2);
    if(c.size() == 0){
        for(int i = 0; i < L; i++){
            double tmp = 0.0;
            for(int j = 1; j < N; j++){
                double tmp2 = 0.0;
                for(int k = 1; k < L; k++){
                    tmp2 += data.at(L*(j - 1) + k - 1);
                }
                tmp += L * data.at(L*(j - 1) + i) / tmp2;
            }
            c.push_back(tmp / N);
        }
    }
    vector<double> c1(c.begin(), c.end());
    for(int i = L; i < data.size(); i++){
        rmse += pow(s + b + c1.at((i - L + 1) % L) - data.at(i), 2);
        double s_prev = s, b_prev = b;
        s = param[0] * (data.at(i) - c1.at(i - L)) + (1 - param[0]) * (s + b);
        b = param[1] * (s - s_prev) + (1 - param[1]) * b;
        c1.push_back(param[2] * (data.at(i) - s_prev - b_prev) + (1 - param[2]) * c1.at(i - L));
    }
    double* predicted = new double[length];
    predicted[0] = s + b + c1.at((data.size() - L) % L);
    for(int i = 1; i < length; i++){
        predicted[i] = s + (i + 1) * b + c1.at((data.size() - L + i) % L);
    }
    return predicted;
}
