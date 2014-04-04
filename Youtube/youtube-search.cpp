// Youtube Script Created Steven Hickson
#define VERSION "v1.0.1"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/regex.hpp>
#include "youtube-search.h"

using namespace std;
using namespace boost;

void PrintUsage(void);

int main(int argc, char *argv[]) {
    Youtube tube;

    if(argc < 2) {
        PrintUsage();
        return -1;
    }

    tube.debug = 1;

    string search = "";
    for(int i = 1; i < argc; ++i) {
        if(i != 1)
            search += "+";
        search += string(argv[i]);
    }
    if (tube.Init()) {
        cout << "Curl failed to initialize. Dying.\n";
        return -1;
    }
    string link;
    tube.Search(search, &link, true);
    tube.PlayVideo(link, true);
    return 0;
}

void PrintUsage(void) {
    printf("Just type what you want it to search just like on the website.\nEx: youtube-search Somebody that I used to know\n");
    printf("Copyright GPL v3 Steven Hickson 2013\n");
}

Youtube::Youtube() {
    hcurl = NULL;
    debug = 0;
    version = VERSION;
}

Youtube::~Youtube() {
    if (hcurl) curl_easy_cleanup(hcurl);
}

//Searches for 1st download link on piratebay.gl

int Youtube::Search(string query, string *link, bool verify) {
    if(!init)
        Init();
    //technically this should never happen now.
    if (!hcurl) {
        cout << "hcurl is NULL.  Did you forget to call Init()?\n";
        return -1;
    }

    string search = "http://www.youtube.com/results?search_query=";
    search += query;
    curl_easy_setopt(hcurl, CURLOPT_URL, search.c_str());
    curlbuf.clear();
    cr = curl_easy_perform(hcurl);
    if (cr != CURLE_OK) {
        cout << "curl() error on getting link: " << errorbuf << endl;
        return -3;
    }

    if (debug & 2) cout << "\nLink curlbuf: [" << curlbuf << "]\n";

    regex rexp("\"/watch\\?([a-z0-9A-Z=]+)\" class"); cmatch m;
    if (regex_search(curlbuf.c_str(), m, rexp)) {
        string t = "http://youtube.com/watch?";
        t += string(m[1]);
        if (verify)
            printf("%s\n", t.c_str());
        *link = t;
        return 0;
    } else {
        cout << "Could not find video. Try again.\n";
        return -1;
    }

    return 0;
}

int Youtube::PlayVideo(string video, bool verify) {
    if(!init)
        Init();
    FILE *pf;
    //This seems like a lot of space to use to format the string but it keeps it well organized for me
    string command = "youtube-safe ";
    command += "\"";
    command += video;
    command += "\"";
    if(verify) printf("%s\n",command.c_str());
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

int Youtube::Init(void) {
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
    return 0;
}

int Youtube::CurlWriter(char *data, size_t size, size_t nmemb, string *buffer) {
    if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        return size*nmemb;
    }
    return 0;
}

