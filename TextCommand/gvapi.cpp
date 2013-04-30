// GoogleVoice API. v1.0.1 Steven Hickson
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
//#include <boost/regex.hpp>
#include "gvoice.h"

using namespace std;
//using namespace boost;

void PrintUsage(int verbose=0);

int main(int argc, char *argv[])
{
	GoogleVoice gv;
	int a,d,r;
    bool check_sms = false, send_sms = false, delete_sms = false, return_rnr = false;
    string keyword="";
    string rnr="";
	string an,am;
	string au,ap;		// These can be set as your GV user/passwd (so they dont have to be specified via CLI each time).
						// eg: au="mygmailuser"; ap="mysecretpasswd";

	while((a=getopt(argc,argv, "d::hvu:p:rcol:k:n:m:")) != -1)
	{
		switch(a)
		{
			case 'h':	// help
				PrintUsage(1); return 0;
			case 'v':	// version.
				printf("GoogleVoice API by Steven Hickson.  %s (%s).  http://code.google.com/p/stevenhickson-code/\n\n", "1.0.1", BUILDTS);
				return 0;
			case 'd':	// debug mode.
				if(optarg) d=atoi(optarg); else d=1;
				gv.debug=d;
				break;
			case 'u':	// username
				au=optarg;
				break;
			case 'p':	// password
				ap=optarg;
				break;
			case 'n':	// number
				an=optarg;
				break;
			case 'm':	// message
				am=optarg;
                send_sms = true;
				break;
            case 'r':   // recieve messages (delete)
                check_sms = true;
                delete_sms = true;
                break;
            case 'c':   // check messages (mark as read)
                check_sms = true;
                break;
            case 'k':   // keyword for checking messages
                keyword = optarg;
                break;
            case 'l' :  // login using given rnr credentials
                rnr = optarg;
                break;
            case 'o' :  // output rnr credentials
                return_rnr = true;
                break;
			case '?':
                printf("Unknown Case\n");
				if(gv.debug) PrintUsage(); return 0;
		}
	}
	if(au.empty() || ap.empty() || (!check_sms && !send_sms)) {PrintUsage(1); return 1;}

	//printf("GoogleVoice API by mm_202.\n");

	if(gv.Init()) {cout << "GoogleVoice() failed to initialize.  Blaming curl.  Dying.\n"; return -1;}
    
    if(rnr == "")
	    r = gv.Login(au,ap);
    else
        gv.Set_rnr(rnr);

	if((gv.debug&1) || r) printf("gv.Login() returned %d.\n\n", r);
    
    if(check_sms) {
        string results;
        string number = "";
        if(!an.empty())
            number += an;
        r = gv.CheckSMS(results,number,keyword,delete_sms);
        printf("%s\n",results.c_str());
        if(!r && gv.debug) printf("SMS Checked Successfully.\n");
    } else {
	    r = gv.SendSMS(an,am);
	    if((gv.debug&2) || r) printf("gv.SendSMS() returned %d.\n\n", r);
	    if(!r) printf("SMS send successfully.\n");
    }
    
    if(return_rnr) {
        gv.Get_rnr(rnr);
        printf("\nRNR: %s\n",rnr.c_str());
    }
	return r;
}

void PrintUsage(int verbose)
{
	printf("Usage: %s -hdv -u [username] -p [password] -c -r -k -n [number] -m [textmsg]\n", "gvapi");	//argv[0]);
	if(!verbose) return;
	printf("\t-v:\t\tGet version information.\n");
	printf("\t-d{n}:\t\tSet debug (verboseness).  If debug value {n} not specified, defaults to 1.\n\t\t\tdebug bits: 1=output GV calls. 2=dump GV call results.\n");
}


