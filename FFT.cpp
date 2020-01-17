#include <stdio.h>
#include <string.h>
#include <iostream>
#include <complex.h>
#include <vector>
#include <math.h>
#include "MODataUtil.cpp"
#include "gnuplot_i.hpp"

using namespace std;

vector<complex<double> > fft(vector<complex<double> > x, int N){
    vector<complex<double> > X(N);
    if(N == 1){
        X.at(0) = x.at(0);
    } else {
        vector<complex<double> > oddori, evenori;
        for(int i = 0; i < N; i++){
            if(i % 2 == 0){ // even
                evenori.push_back(x.at(i));
            } else { //odd
                oddori.push_back(x.at(i));
            }
        }
        vector<complex<double> > odd = fft(oddori, N/2);
        vector<complex<double> > even = fft(evenori, N/2);
        for(int i = 0; i < N / 2; i++){
            complex<double> evenv = even.at(i);
            complex<double> oddv = odd.at(i);
            complex<double> t = polar(1.0, -2 * M_PI * i / N) * oddv;
            X.at(i) = evenv + t;
            X.at(i + N / 2) = evenv - t;
        }
    }
    return X;
}

vector<complex<double> > ifft(vector<complex<double> > x){
    for(int i = 0; i < x.size(); i++){
        x.at(i) = conj(x.at(i));
    }
    vector<complex<double> > X = fft(x, x.size());
    for(int i = 0; i < X.size(); i++){
        X.at(i) = conj(X.at(i));
        X.at(i) /= X.size();
    }
    return X;
}

vector<double> fftCombine(vector<complex<double> > x, double loops){
    int length = (int) (x.size() * loops);
    vector<double> result(length);
    for(int i = 0; i < length; i++){
        result.at(i) = 0.0;
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
            double ind = loops / length * M_PI * 2 * j;
            result.at(j) += real(tmp) * cos(i * ind) - imag(tmp) * sin(i * ind);
        }
    }
    return result;
} 

int main(){
    MOStation Armagh = MODataUtil::loadStationFromFile("Data/MetOffice/armaghdata.txt");
    vector<double> tempAvg = Armagh.getTempAvg();
    int offset = tempAvg.size() - 100;
    vector<double> tempAvgpro;
    vector<double> tempAvg2;
    vector<complex<double> > tempAvgComplex;
    for(int i = offset; i < tempAvg.size(); i++){
        // complex<double> tmp(tempAvg.at(i) - tempAvg.at(i - 1), 0);
        complex<double> tmp(tempAvg.at(i));
        if(isnan(real(tmp))){
            continue;
        }
        tempAvgComplex.push_back(tmp);
        tempAvgpro.push_back(real(tmp));
        tempAvg2.push_back(tempAvg.at(i));
    }
    vector<complex<double> > X = fft(tempAvgComplex, tempAvgComplex.size());
    // vector<complex<double> > Xi = ifft(X);
    // vector<double> xii(Xi.size());
    // for(int i = 0; i < xii.size(); i++){
    //     xii.at(i) = real(Xi.at(i));
    // }
    vector<double> result = fftCombine(X, 1.1);

    vector<double> predicted(result.size() + 1);
    predicted.at(0) = tempAvg.at(offset - 1);
    for(int i = 1; i < predicted.size(); i++){
        predicted[i] = predicted[i - 1] + (result[i - 1]);
    }

    Gnuplot ArmaghPlot("Armagh");
    ArmaghPlot.set_style("lines").plot_x(result, "pre diff");
    ArmaghPlot.set_style("lines").plot_x(tempAvgpro, "raw diff");
    // ArmaghPlot.set_style("lines").plot_x(xii, "");

    Gnuplot g2;
    g2.set_style("lines").plot_x(tempAvg2, "raw");
    g2.set_style("lines").plot_x(predicted, "pre");

    ArmaghPlot.showonscreen();
    cout << result.size() << " " << tempAvgpro.size() << endl;
    cout << endl << "Press ENTER to continue..." << endl;
    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get(); 
    return 0;
}