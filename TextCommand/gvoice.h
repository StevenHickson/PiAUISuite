#ifndef __GVOICE_H__
#define __GVOICE_H__

#include <string>
#include <curl/curl.h>
#include <time.h>

#define DTFILE "/dev/shm/time"

using namespace std;

class GoogleVoice {
	CURL *hcurl;
	CURLcode cr;
	string email,passwd,myNum;
	string rnr_se;			// Session token that Google needs. 
protected:
	string version;
public:
	char errorbuf[CURL_ERROR_SIZE];
	string curlbuf;
    string contact_buf;

	int loggedin;
	int debug;				// Temp flag used to mostly dump the contents of curlbuf.
	GoogleVoice();
	~GoogleVoice();

	int Init(void);
	int Login();
	int Login(string login, string passwd);
	//int Logout(void);
	
    int GetContactInfo();
	int SendSMS(string number, string msg);
    int CheckSMS(string &results, string number, string keyword, bool delete_sms);
    int MarkAsRead(string msg_id);
    int DeleteSMS(string msg_id);
    int BlockSMS(string msg_id);
    int CallNumber(string to, string from);
    void Get_rnr(string &rnr) {
        rnr = rnr_se;
    }
    void Set_rnr(string rnr) {
        rnr_se = rnr;
        loggedin = 1;
    }
    void Logout(void) {
        rnr_se = "";
        loggedin = 0;
    }

	static int CurlWriter(char *data, size_t size, size_t nmemb, string *buffer);
};

#endif
