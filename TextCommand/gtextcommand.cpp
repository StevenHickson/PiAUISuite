// Text Message Commander Steven Hickson 2013.
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include "gvoice.h"

using namespace std;
using namespace boost;

#define DATA_SIZE 200

int main(int argc, char *argv[])
{
	GoogleVoice gv;
	int r;
    if(argc == 2) gv.debug = 3;
    //read username, password, valid number, and keyword from safe file
	FILE *fp;
    char *passPath;
    passPath = getenv("HOME");
    if(passPath == NULL) {
        printf("Could not get $HOME\n");
        return -1;
    }
    string path = string(passPath);
    path += "/.gtext";
    fp = fopen(path.c_str(),"r");
    if(!fp) {cout << "Couldn't open password file. Dying\n"; return -1;}
    char buf1[100];
    char buf2[100];
    char buf3[100];
    char buf4[100];
    fscanf(fp,"%s\n%s\n%s\n%s",buf1,buf2,buf3,buf4);
    fclose(fp);
    
    string username = string(buf1);
    string password = string(buf2);
    string keyword = string(buf3);
    string number = string(buf4);
    
    //Log in to google voice
    if(gv.Init()) {cout << "GoogleVoice() failed to initialize.  Blaming curl.  Dying.\n"; return -1;}
	r = gv.Login(username,password);
	if((gv.debug&1) || r) printf("gv.Login() returned %d.\n\n", r);
    
    //Check SMS
    string results;
    r = gv.CheckSMS(results,number,keyword,true);
    //printf("%s\n",results.c_str());
    if(!r && gv.debug) printf("SMS Checked Successfully.\n");
	
    //run given command
    if(!results.empty()) {
        FILE *pf;
        pf = popen(results.c_str(),"r");

        if(!pf) {cout << "Could not open command pipe. Dying\n"; return -1;}
        
        //Grab data from process execution
        char buffer[DATA_SIZE];
        string message = "";
        while(!feof(pf)) {
            if(fgets(buffer, DATA_SIZE, pf) != NULL) {
                message += string(buffer);
            }
        }
        //printf("%d: %s\n",count,message.c_str());

        if (pclose(pf) != 0) {cout << "Could not close command pipe. Dying\n"; return -1;}
        replace_all(message, "\t", " ");
        //Send return SMS
        int mLen = message.length();
        if(mLen <= 320)
            r = gv.SendSMS(number,message);
        else {
            //I need to split this into multiple messages
            string::iterator it = message.begin();
            int count = 0;
            string section = "";
            while(it != message.end()) {
                section += *it;
                ++it;
                ++count;
                if(count >= 319) {
                    r = gv.SendSMS(number, section);
                    count = 0;
                    section.clear();
                }
            }
            r = gv.SendSMS(number, section);
        }
	    if((gv.debug&2) || r) printf("gv.SendSMS() returned %d.\n\n", r);
	    if(gv.debug&1 && !r) printf("SMS send successfully.\n");
    }    
	return r;
}

