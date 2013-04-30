#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "downconn.h"

using namespace std;

void PrintUsage(void);

int main(int argc, char *argv[]) {
    Downloader down;

    if(argc < 2) {
        PrintUsage();
        return -1;
    }

    down.debug = 1;

    string search = "";
    for(int i = 1; i < argc; ++i) {
        if(i != 1)
            search += "%20";
        search += string(argv[i]);
    }
    if (down.Init()) {
        cout << "Curl failed to initialize. Dying.\n";
        return -1;
    }
    string link;
    down.Search(search, &link, false);
    down.DownloadTorrent(link);
    return 0;
}

void PrintUsage(void) {
    printf("Just type what you want it to download like you would search on a website.\nEx: download Futurama S07E08 HD\n");
    printf("Copyright GPL v3 Steven Hickson 2013\n");
}


