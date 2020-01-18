#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "curl/curl.h"

using namespace std;

class DataUtil{
public:
    static int downloadFile(string url, string outfileName);
    void downloadDataFile(string fileName);
private:
    static size_t writeCallback(void*, size_t, size_t, FILE*);
};

size_t DataUtil::writeCallback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int DataUtil::downloadFile(string url, string outfileName){
    CURL *curl;
    FILE* fp = fopen(outfileName.c_str(), "wb");
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DataUtil::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_perform(curl);
    }
    fclose(fp);
    curl_global_cleanup();
    return 0;
}

void DataUtil::downloadDataFile(string fileName){
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
    urlFile.close();
}