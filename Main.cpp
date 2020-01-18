#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "MODataUtil.cpp"
#include "FFT.cpp"

using namespace std;

int main(int argc, char** argv){
    MOStation station = MOStation::loadStationFromFile("Data/MetOffice/bradforddata.txt");
    FFT fft(station.getTempAvg());
    fft.predict(60);
    fft.plotResult();
    return 0;
}
