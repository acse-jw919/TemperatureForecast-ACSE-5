#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "MODataUtil.cpp"
#include "FFT.cpp"

using namespace std;

int main(int argc, char** argv){
    string stationName(argv[1]);
    int periods = atoi(argv[2]);
    MOStation station = MOStation::loadStationFromFile("Data/MetOffice/shawburydata.txt");
    FFT fft(station.getTempAvg());
    fft.predict(periods);
    fft.plotResult();
    return 0;
}
