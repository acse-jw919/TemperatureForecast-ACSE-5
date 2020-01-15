#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include "MODataUtil.cpp"

#define MAXPRE 1000

using namespace std;

class ExponentialSmoothing{
public:
    static double basicES(vector<double> data);
    static double doubleES(vector<double> data);
    static double tripleES(vector<double> data);
private:
    static double optimize(vector<double> data, int nparam, double (*stepFunc)(vector<double>, double&, double*));
    static double basicStep(vector<double> data, double &rmse, double *param);
    static double doubleStep(vector<double> data, double &rmse, double *param);
    static double tripleStep(vector<double> data, double &rmse, double *param);
};

double ExponentialSmoothing::basicES(vector<double> data){
    double* param = new double[1];
    param[0] = optimize(data, 1, basicStep);
    double rmse;
    double predicted = basicStep(data, rmse, param);
    return predicted;
}

double ExponentialSmoothing::doubleES(vector<double> data){
    double* param = new double[1];
    param[0] = optimize(data, 1, doubleStep);
    double rmse;
    double predicted = doubleStep(data, rmse, param);
    return predicted;
}

double ExponentialSmoothing::tripleES(vector<double> data){
    double* param = new double[1];
    param[0] = optimize(data, 1, tripleStep);
    double rmse;
    double predicted = tripleStep(data, rmse, param);
    return predicted;
}

double ExponentialSmoothing::optimize(vector<double> data, int nparam, double (*stepFunc)(vector<double>, double&, double*)){
    double* param = new double[nparam];
    double best_alpha = 1.0 / MAXPRE;
    double best_rmse = 10000;
    for(int i = 1; i < MAXPRE; i++){
        param[0] = double(i) / MAXPRE;
        double rmse = 0.0;
        double predicted = stepFunc(data, rmse, param);
        rmse = sqrt(rmse / data.size());
        if(rmse < best_rmse){
            best_alpha = param[0];
            best_rmse = rmse;
        }
    }
    cout << "RMSE: " << best_rmse << " alpha: " << best_alpha << endl;
    return best_alpha;
}

double ExponentialSmoothing::basicStep(vector<double> data, double &rmse, double *param){
    double s = data.at(0);
    for(int i = 1; i < data.size(); i++){
        s = param[0] * data.at(i) + (1 - param[0]) * s;
        rmse += pow(s - data.at(i), 2);
    }
    return s;
}

double ExponentialSmoothing::doubleStep(vector<double> data, double &rmse, double *param){
    double s1 = data.at(0);
    double s2 = data.at(0);
    for(int i = 1; i < data.size(); i++){
        s1 = param[0] * data.at(i) + (1 - param[0]) * s1;
        s2 = param[0] * s1 + (1 - param[0]) * s2;
        rmse += pow((2*s1 - s2) + param[0] / (1 - param[0]) * (s1 - s2) - data.at(i), 2);
    }
    return (2*s1 - s2) + param[0] / (1 - param[0]) * (s1 - s2); 
}

double ExponentialSmoothing::tripleStep(vector<double> data, double &rmse, double *param){
    double s1 = data.at(0);
    double s2 = data.at(0);
    double s3 = data.at(0);
    for(int i = 1; i < data.size(); i++){
        s1 = param[0] * data.at(i) + (1 - param[0]) * s1;
        s2 = param[0] * s1 + (1 - param[0]) * s2;
        s3 = param[0] * s2 + (1 - param[0]) * s3;
        double A = (3*s1 - 3*s2 + s3);
        double B = 0.5 * param[0] / pow(1-param[0], 2) * ((6-5*param[0])*s1 - 2*(5-4*param[0])*s2 + (4-3*param[0])*s3);
        double C = 0.5 * pow(param[0], 2) / pow(1-param[0], 2) * (s1 - 2*s2 + s3);
        rmse += pow(A + B + C - data.at(i), 2);
    }
    double A = (3*s1 - 3*s2 + s3);
    double B = 0.5 * param[0] / pow(1-param[0], 2) * ((6-5*param[0])*s1 - 2*(5-4*param[0])*s2 + (4-3*param[0])*s3);
    double C = 0.5 * pow(param[0], 2) / pow(1-param[0], 2) * (s1 - 2*s2 + s3);
    return A + B + C;
}

int main(){
    MOStation Armagh = MODataUtil::loadStationFromFile("Data/MetOffice/bradforddata.txt");
    double predicted1 = ExponentialSmoothing::basicES(Armagh.getTempHigh());
    double predicted2 = ExponentialSmoothing::doubleES(Armagh.getTempHigh());
    double predicted3 = ExponentialSmoothing::tripleES(Armagh.getTempHigh());
    cout << "Basic Temperature High: " << predicted1 << endl;
    cout << "Double Temperature High: " << predicted2 << endl;
    cout << "Triple Temperature High: " << predicted3 << endl;
    return 0;
}