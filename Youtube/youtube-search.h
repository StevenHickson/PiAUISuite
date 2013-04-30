#ifndef __DOWNCONN_H__
#define __DOWNCONN_H__

#include <string>
#include <curl/curl.h>

using namespace std;

#define DATA_SIZE 200

class Youtube {
private:
    CURL *hcurl;
    CURLcode cr;
    bool use_pass, init;
protected:
    string version;
public:
    char errorbuf[CURL_ERROR_SIZE];
    string curlbuf;

    int debug; 
    Youtube();
    ~Youtube();

    int Search(string query, string *link, bool verify);
    int PlayVideo(string video, bool verify);

    int Init(void);

    static int CurlWriter(char *data, size_t size, size_t nmemb, string *buffer);
};

#endif
