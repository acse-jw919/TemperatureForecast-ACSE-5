#include <stdio.h>
#include <string.h>
#include <iostream>
#include <complex.h>
#include <vector>
#include <math.h>
#include "MODataUtil.cpp"
#include "gnuplot_i.hpp"

using namespace std;

typedef complex<double> Complex;

class FFT{
private:
    int size;
    vector<double> rawData;
    vector<double> processedData;
    vector<Complex> rawComplex;
    vector<double> prediction;
    double rmse;
public:
    FFT(vector<double>);
    void predict(int);
    void plotResult();
private:
    vector<Complex> forwardFFT(vector<Complex>, int);
    vector<Complex> inverseFFT(vector<Complex>);
    void combineFFT(vector<Complex>, double);
};

FFT::FFT(vector<double> data){
    size = data.size();
    for(int i = size - pow(2, 8) - 1; i < size; i++){
        Complex tmp(log(data[i]), 0.0);
        if(isnan(real(tmp))){
            continue;
        }
        rawData.push_back(data[i]);
        processedData.push_back(log(data[i]));
        rawComplex.push_back(tmp);
    }
    size = pow(2, 8);
}

void FFT::predict(int periodAfter){
    double loops = 1 + (double) periodAfter / (double) size;
    vector<Complex> fftValues = forwardFFT(rawComplex, size);
    combineFFT(fftValues, loops);
    rmse = 0.0;
    for(int i = 0; i < prediction.size(); i++){
        prediction[i] = exp(prediction[i]);
        if(i < size){
            rmse += pow(prediction[i] - rawData[i], 2);
        }
    }
    rmse = sqrt(rmse / size);
    cout << "RMSE: " << rmse << endl;
}

void FFT::plotResult(){
    Gnuplot predPlot;
    predPlot.set_style("lines").plot_x(prediction, "Prediction");
    predPlot.set_style("lines").plot_x(rawData, "Raw");
    predPlot.showonscreen();
    cout << endl << "Press ENTER to continue..." << endl;
    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get(); 
}

vector<Complex> FFT::forwardFFT(vector<Complex> x, int N){
    vector<Complex> X(N);
    if(N == 1){
        X[0] = x[0];
    } else {
        vector<Complex> oddori, evenori;
        for(int i = 0; i < N; i++){
            if(i % 2 == 0){ // even
                evenori.push_back(x.at(i));
            } else { //odd
                oddori.push_back(x.at(i));
            }
        }
        vector<complex<double> > odd = forwardFFT(oddori, N/2);
        vector<complex<double> > even = forwardFFT(evenori, N/2);
        for(int i = 0; i < N / 2; i++){
            Complex evenv = even[i], oddv = odd[i];
            Complex t(0.0, -2 * M_PI * i / N);
            t = exp(t) * oddv;
            X[i] = evenv + t;
            X[i + N / 2] = evenv - t;
        }
    }
    return X;
}

vector<Complex> FFT::inverseFFT(vector<Complex> x){
    for(int i = 0; i < x.size(); i++){
        x[i] = conj(x[i]);
    }
    vector<Complex> X = forwardFFT(x, x.size());
    for(int i = 0; i < X.size(); i++){
        X[i] = conj(X[i]);
        X[i] /= X.size();
    }
    return X;
}

void FFT::combineFFT(vector<Complex> x, double loops){
    int length = (int) (x.size() * loops);
    prediction.clear();
    prediction.resize(length);
    for(int i = 0; i < length; i++){
        prediction[i] = 0.0;
    }
    for(int i = 0; i < x.size(); i++){
        x.at(i) /= x.size();
    }
    for(int i = 0; i < x.size() / 2; i++){
        complex<double> tmp = x.at(i);
        if(i != 0){
            tmp *= 2;
        }
        for(int j = 0; j < length; j++){
            double ind = (double) j * loops / length * M_PI * 2;
            prediction[j] += real(tmp) * cos(i * ind) - imag(tmp) * sin(i * ind);
        }
    }
} 

int main(){
    MOStation s1 = MODataUtil::loadStationFromFile("Data/MetOffice/heathrowdata.txt");
    vector<double> s11 = s1.getTempAvg();
    FFT s111(s11);
    s111.predict(60);
    s111.plotResult();
    return 0;
}