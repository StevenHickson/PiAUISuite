#ifndef __DOWNCONN_H__
#define __DOWNCONN_H__

#include <string>
#include <curl/curl.h>

using namespace std;

#define DATA_SIZE 200

class Downloader {
private:
    CURL *hcurl;
    CURLcode cr;
    string host;
    string port;
    string username;
    string password;
    bool use_pass, init;
protected:
    string version;
public:
    char errorbuf[CURL_ERROR_SIZE];
    string curlbuf;

    int debug; 
    Downloader();
    ~Downloader();

    int Search(string download, string *link, bool verify);
    int DownloadTorrent(string torrent);

    int Init(void);
    int SetTransmissionParams(void);

    static int CurlWriter(char *data, size_t size, size_t nmemb, string *buffer);
};

#endif
