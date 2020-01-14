#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "DataUtil.cpp"

using namespace std;

class MODataUtil{
public:
    static void downloadDataFile(string fileName);
    static void loadStationData(string stationName);
};

void MODataUtil::downloadDataFile(string fileName){
    ifstream urlFile(fileName);
    string url;
    string dataFolder("Data/MetOffice/");
    while(getline(urlFile, url)){
        int lastIt = url.find_last_of('/') + 1;
        string station = url.substr(lastIt, url.length() - lastIt - 1);
        if(station.find(' ', 0) != -1){
            continue;
        }
        cout << station << endl;
        DataUtil::downloadFile(url, dataFolder + station);
    }
}

int main(int argc, char **argv){
    
    /* Download from URL lists */
    // MODataUtil::downloadDataFile("Data/MetOfficeData.txt");



    return 0;
}