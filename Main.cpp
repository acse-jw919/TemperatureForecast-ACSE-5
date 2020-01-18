#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include "MODataUtil.cpp"
#include "FFT.cpp"

using namespace std;

void listFiles(string dirPath){
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirPath.c_str())) != NULL) {
        int i = 0;
        while ((ent = readdir (dir)) != NULL) {
            string fileName(ent->d_name);
            int pos = fileName.find("data.txt");
            if(pos != -1){
                printf("%15s\t", fileName.substr(0, pos).c_str());
                if(++i % 5 == 0){
                    cout << endl;
                }
            }
        }
        cout << endl;
        closedir (dir);
    } else {
        perror ("");
    }
}

int main(int argc, char** argv){
    string dataDir = "Data/MetOffice/";
    if(argc <= 2){
        cout << "Require both a station and a period value" << endl;
        cout << "Available stations: " << endl;
        listFiles(dataDir);
        cout << "For example: " << argv[0] << " heathrow 12" << endl;
        return 0;
    }
    string stationName(argv[1]);
    int periods = atoi(argv[2]);
    string filename = dataDir + stationName + "data.txt";
    if(FILE *file = fopen(filename.c_str(), "r")){
        fclose(file);
        MOStation station = MOStation::loadStationFromFile(filename);
        FFT fft(station.getTempAvg());
        fft.predict(periods);
        fft.plotResult();
    } else {
        cout << "Station " << stationName << " Not Found" << endl;
        cout << "Available stations: " << endl;
        listFiles(dataDir);
    }
    return 0;
}
