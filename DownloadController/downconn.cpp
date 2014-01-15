// Downloader Script Created Steven Hickson
#define VERSION "v1.0.1"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/regex.hpp>
#include "downconn.h"

using namespace std;
using namespace boost;

Downloader::Downloader() {
    hcurl = NULL;
    debug = 0;
    version = VERSION;
}

Downloader::~Downloader() {
    if (hcurl) curl_easy_cleanup(hcurl);
}

//Searches for 1st download link on thepiratebay.s

int Downloader::Search(string download, string *link, bool verify) {
    if(!init)
        Init();
    //technically this should never happen now.
    if (!hcurl) {
        cout << "hcurl is NULL.  Did you forget to call Init()?\n";
        return -1;
    }
    
    //Going back to using https again
    string torr = "http://thepiratebay.se/search/";
    torr += download;
    torr += "/0/7/0";
    curl_easy_setopt(hcurl, CURLOPT_URL, torr.c_str());
    curlbuf.clear();
    cr = curl_easy_perform(hcurl);
    if (cr != CURLE_OK) {
        cout << "curl() error on getting link: " << errorbuf << endl;
        return -3;
    }

    if (debug & 2) cout << "\nLink curlbuf: [" << curlbuf << "]\n";

    regex rexp("<a href=\"magnet:(.+?)\" ");
    cmatch m;
    if (regex_search(curlbuf.c_str(), m, rexp)) {
        string t = "magnet:";
        t += string(m[1]);
        if (verify)
            printf("%s\n", t.c_str());
        *link = t;
        return 0;
    } else {
        cout << "Could not find torrent. Try again.\n";
        return -1;
    }

    return 0;
}

int Downloader::DownloadTorrent(string torrent) {
    if(!init)
        Init();
    FILE *pf;
    //This seems like a lot of space to use to format the string but it keeps it well organized for me
    string command = "transmission-remote ";
    command += host;
    command += ":";
    command += port;
    if(use_pass) {
        command += " -n ";
        command += username;
        command += ":";
        command += password;
    }
    command += " -a \"";
    command += torrent;
    command += "\"";
    pf = popen(command.c_str(), "r");

    if (!pf) {
        cout << "Could not open command pipe. Dying\n";
        return -1;
    }

    //Grab data from process execution
    char buffer[DATA_SIZE];
    string message = "";
    while (!feof(pf)) {
        if (fgets(buffer, DATA_SIZE, pf) != NULL) {
            message += string(buffer);
        }
    }
    printf("%s\n",message.c_str());

    if (pclose(pf) != 0) {
        cout << "Could not close command pipe. Dying\n";
        return -1;
    }

}

int Downloader::Init(void) {
    hcurl = curl_easy_init();
    if (!hcurl) return -1;
    curl_easy_setopt(hcurl, CURLOPT_ERRORBUFFER, errorbuf);
    curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, CurlWriter);
    curl_easy_setopt(hcurl, CURLOPT_WRITEDATA, &curlbuf);

    curl_easy_setopt(hcurl, CURLOPT_HEADER, 0);
    curl_easy_setopt(hcurl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(hcurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.2.8) Gecko/20100804 Gentoo Firefox/3.6.8");
    init = true;
    //curl_easy_setopt(hcurl, CURLOPT_COOKIEJAR, "cookie.txt");
    return SetTransmissionParams();
}

int Downloader::SetTransmissionParams(void) {
    char* home;
    home = getenv("HOME");
    if(home == NULL) {
        printf("Could not get $HOME\n");
        return -1;
    }
    FILE *fp;
    string file = string(home);
    file += "/.down.info";
    fp = fopen(file.c_str(), "r");
    if(!fp) {cout << "Couldn't open password file. Dying\n"; return -1;}
    char buf1[100];
    char buf2[100];
    char buf3[100];
    char buf4[100];
    fscanf(fp,"%s\n%s\n%s\n%s",buf1,buf2,buf3,buf4);
    fclose(fp);
    
    host = string(buf1);
    port = string(buf2);
    username = string(buf3);
    password = string(buf4);
    if(username.empty() || password.empty())
        use_pass = false;
    else
        use_pass = true;
    return 0;
}

int Downloader::CurlWriter(char *data, size_t size, size_t nmemb, string *buffer) {
    if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        return size*nmemb;
    }
    return 0;
}

