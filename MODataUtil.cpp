#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "gnuplot_i.hpp"

using namespace std;

double string2Double(string str){
    stringstream ss;
    double num;
    ss << str;
    ss >> num;
    return num;
}

class MOStation{
private:
    string stationName;
    map<string, int> monthMap;
    vector<double> tempHigh;
    vector<double> tempLow;
    vector<double> tempAvg;
    vector<int> daysOfAF;
    vector<double> rainfall;
    vector<double> sunshine;
public:
    static MOStation loadStationFromFile(string fileName);
    MOStation(string stationName);
    int getRecordSize();
    void appendARecord(int year, int month, double tH, double tL, int af, double rf, double ss);
    string formatDate(int year, int month);
    map<string, int> getMonthMap() {return monthMap; }
    vector<double> getTempHigh() {return tempHigh; }
    vector<double> getTempLow() {return tempLow; }
    vector<double> getTempAvg() {return tempAvg; }
    vector<int> getDaysOfAF() {return daysOfAF; }
    vector<double> getRainfall() {return rainfall; }
    vector<double> getSunshine() {return sunshine; }
    void printData(int line);
};

MOStation::MOStation(string stationName): stationName(stationName){}

string MOStation::formatDate(int year, int month){
    char targetString[8];
    snprintf(targetString, sizeof(targetString), string("%04d-%02d").c_str(), year, month);
    return string(targetString);
}

int MOStation::getRecordSize(){
    return tempHigh.size();
}

void MOStation::appendARecord(int year, int month, double tH, double tL, int af, double rf, double ss){
    int currentSize = getRecordSize();
    monthMap.insert(pair<string, int>(formatDate(year, month), currentSize + 1));
    tempHigh.push_back(tH);
    tempLow.push_back(tL);
    tempAvg.push_back((tH + tL) / 2);
    daysOfAF.push_back(af);
    rainfall.push_back(rf);
    sunshine.push_back(ss);
}

void MOStation::printData(int line){
    printf("tmax\ttmin\ttavg\tAF\tRain\tSun\n");
    for(int i = 0; i < line && i < getRecordSize(); i++){
        printf("%.2f\t%.2f\t%.2f\t%d\t%.2f\t%.2f\n", tempHigh.at(i), tempLow.at(i),
                tempAvg.at(i), daysOfAF.at(i), rainfall.at(i), sunshine.at(i));
    }
}

MOStation MOStation::loadStationFromFile(string fileName){
    ifstream dataFile(fileName);
    string line;
    int lineNum = 0;
    // Station Name
    getline(dataFile, line);
    MOStation tmpStation(line);
    while(getline(dataFile, line)){
        lineNum++;
        if(line.find("yyyy") != -1){
            getline(dataFile, line);
            lineNum++;
            break;
        }
    }
    int full = lineNum + 1;
    while(getline(dataFile, line)){
        lineNum++;
        if(line.find("---") != -1){
            full = lineNum + 1;
        }
    }
    dataFile.close();
    ifstream dataFile2(fileName);
    for(int i = 0; i < full; i++) getline(dataFile2, line);
    while(getline(dataFile2, line)){
        char *dataLine = new char[line.length() + 1];
        strcpy(dataLine, line.c_str());
        char *splited = strtok(dataLine, " ");
        int year = stoi(string(splited));
        splited = strtok(NULL, " ");
        int month = stoi(string(splited));
        splited = strtok(NULL, " ");
        double tH = string2Double(splited);
        splited = strtok(NULL, " ");
        double tL = string2Double(splited);
        splited = strtok(NULL, " ");
        double af = string2Double(splited);
        splited = strtok(NULL, " ");
        double rf = string2Double(splited);
        splited = strtok(NULL, " ");
        string tmp = string(splited);
        if(splited[tmp.length() - 1] == '*')
            tmp = tmp.substr(0, tmp.length() - 1);
        double ss = string2Double(tmp);
        tmpStation.appendARecord(year, month, tH, tL, af, rf, ss);
    }
    dataFile2.close();
    return tmpStation;
}