// GoogleVoice API.  Created by Steven Hickson ported from mastermind202
#define GV_VERSION "v1.0.1"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/regex.hpp>
#include "gvoice.h"

using namespace std;
using namespace boost;

GoogleVoice::GoogleVoice()		{hcurl=NULL; loggedin=0; debug=0; version=GV_VERSION;}
GoogleVoice::~GoogleVoice()		{if(hcurl) curl_easy_cleanup(hcurl);}

int GoogleVoice::SendSMS(string number, string msg)
{
	if(!hcurl) {cout << "hcurl is NULL.  Did you forget to call Init()?\n"; return -1;}
	if(Login()) return -1;

	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/sms/send/");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	
	string data  = "_rnr_se="+rnr_se;
	data += "&phoneNumber=1"+number;
	data += "&text="+msg;
	
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on sending sms: " << errorbuf << endl; return -3;}
	
	if(debug&2) cout << "\nSendSMS curlbuf: [" << curlbuf << "]\n";

	regex rexp("\"data\":\\{\"code\":(\\d+)\\}"); cmatch m;
	if(regex_search(curlbuf.c_str(), m, rexp)) {string t=m[1]; return atoi(t.c_str());}
	else {cout << "Something went wrong.  Enable debugging.\n"; return -1;}
	
	return -1;
}

int GoogleVoice::MarkAsRead(string msg_id)
{
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/inbox/mark/");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	
	string data = "messages="+msg_id;
    data += "&read=1";
    data += "&_rnr_se="+rnr_se;
	
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on marking sms as read: " << errorbuf << endl; return -3;}
    return 0;
}

int GoogleVoice::GetContactInfo() {
	if(!hcurl) {cout << "hcurl is NULL.  Did you forget to call Init()?\n"; return -1;}
	if(Login()) return -1;

    string curlbuf = contact_buf;
    
    regex rexp("'contacts'"); cmatch m;
    if(regex_search(curlbuf.c_str(), m, rexp)) {
        //printf("Found contacts\n");
        regex rexp2("\"name\":\"([a-zA-Z0-9 -]+)\",\"photoUrl\":\"\",\"phoneNumber\":\"\\+(\\d+)\""); smatch n;
        string::const_iterator  begin = curlbuf.begin(), end = curlbuf.end();
        while(regex_search(begin, end, n, rexp2)) {
            smatch::value_type r = n[1];
            begin = r.second;
            string name = n[1];
            string number = n[2];
            printf("%s==+%s\n",name.c_str(),number.c_str());
        }
    }
    return 0;
}

int GoogleVoice::DeleteSMS(string msg_id)
{
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/inbox/deleteMessages/");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	
	string data = "messages="+msg_id;
    data += "&trash=1";
    data += "&_rnr_se="+rnr_se;
	
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on deleting sms: " << errorbuf << endl; return -3;}
    return 0;
}

int GoogleVoice::BlockSMS(string msg_id)
{
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/inbox/mark/");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	
	string data = "messages="+msg_id;
    data += "&spam=1";
    data += "&_rnr_se="+rnr_se;
	
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on marking sms as read: " << errorbuf << endl; return -3;}
    return 0;
}

int GoogleVoice::CheckSMS(string &results, string number, string keyword, bool delete_sms)
{
	if(!hcurl) {cout << "hcurl is NULL.  Did you forget to call Init()?\n"; return -1;}
	if(Login()) return -1;

    results = "";
	
    curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/inbox/recent/unread");
    //Doesn't need any post information since it's getting the XML file (focus on unread as it saves a ton of memory/time)

	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on getting sms: " << errorbuf << endl; return -3;}
	
	if(debug&2) cout << "\nSendSMS curlbuf: [" << curlbuf << "]\n";
    string orig = curlbuf;

	regex rexp("\"unread\":(\\d+)"); cmatch m;
	if(regex_search(curlbuf.c_str(), m, rexp)) {
        string t=m[1];
        int num_unread = atoi(t.c_str());
	    //cout << num_unread << "\n" << curlbuf.c_str() << "\n";
        if(num_unread == 0) {
            return 0;
        } else {
            string msg_id;
            string reg_check="\"id\":\"([a-z0-9]+)\",";
            if(number != "") {
                reg_check += "\"phoneNumber\":\"\\+";
                reg_check += number;
                reg_check += "\"";
            }
            regex rexp3(reg_check); cmatch q;
	        if(regex_search(orig.c_str(), q, rexp3)) {
                msg_id=q[1];
                //cout << msg_id.c_str() << "\n";
            } else {
                //cout << "Couldn't read msg_id.\n"; 
                return -1;
            }
            //printf("First check passed!\n");
            //reg_check = number;
            reg_check ="class=\"gc-message-sms-text\">";
            if(keyword != "") {
                reg_check+=keyword;
                reg_check+=" ";
            }
            reg_check+="(.+?)</span>";
            regex rexp2(reg_check); cmatch n;
            //regex rexp2("\"gc-message-sms-text\">([\\/ / /|/.\"><A-Za-z0-9/-]+)<\\/span>"); cmatch n;
            string::const_iterator  begin = curlbuf.begin(), end = curlbuf.end();
            smatch  p;
            while(regex_search(begin, end, p, rexp2)) {
                smatch::value_type  r = p[1];
                begin = r.second;
                string t2 = p[1];
                //results += t2;
                //results += "\n";
                results = t2;
            }
            if(results != "") {
                //printf("Second check passed!\n");
                MarkAsRead(msg_id);
                reg_check = "gc-message-sms-text(.+?)phoneNumber";
                regex rexpPhony(reg_check); cmatch f;
                if(regex_search(orig.c_str(), f, rexpPhony)) {
                    //someone is trying to spoof me and this is invalid
                    SendSMS(number,string("Spoof attack!"));
                    //BlockSMS(msg_id);
                    //DeleteSMS(msg_id);
                    results.clear();
                    //By exiting here, they will never be able to spoof the phoneNumber query
                    return -4;
                } else if(delete_sms)
                    DeleteSMS(msg_id);
                //printf("Third check passed!\n");
            }
        }
        return num_unread;
    } else {cout << "Something went wrong.  Enable debugging.\n"; return -1;}
	
	return 0;

}

int GoogleVoice::Login(string email, string passwd, bool get_contacts)
{
	if(email.length()<1 || email.length()<1) return -1;
	this->email=email; this->passwd=passwd;
	return Login(get_contacts);
}

int GoogleVoice::Login(bool get_contacts)
{
	if(!hcurl) {cout << "hcurl is NULL.  Did you forget to call Init()?\n"; return -1;}
	if(loggedin) return 0;

	regex rexp; cmatch m;
	string post, galx;

	// Get GLAX token.
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://accounts.google.com/ServiceLogin?passive=true&service=grandcentral");
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() Error: " << errorbuf << endl; return -1;}

	rexp = "name=\"GALX\"\\s*value=\"([^\"]+)\"";
	if(regex_search(curlbuf.c_str(), m, rexp)) 
	{ 
		galx=m[1];
		if(debug&1) cout << "Got GALX session token: " << galx << endl;

	} else {cout << "Failed to find GALX token.\n"; return -2;}

	// Login and get rnr_se token.
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://accounts.google.com/ServiceLoginAuth?service=grandcentral");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
    post  = "Email="+email;
	post += "&Passwd="+passwd;
	post += "&continue=https://www.google.com/voice/account/signin";
	post += "&service=grandcentral&GALX="+galx;
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, post.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() Error: " << errorbuf << endl; return -3;}

    if(get_contacts)
        contact_buf = curlbuf;

	if(debug&2) cout << "\nLogin() curlbuf: [" << curlbuf << "]\n";

	rexp = "name=\"_rnr_se\".*?value=\"(.*?)\"";
	if(regex_search(curlbuf.c_str(), m, rexp)) 
	{ 
		rnr_se=m[1]; loggedin=1;
		if(debug&1) cout << "Got rnr_se session token: " << rnr_se << endl;
	}
	else
	{
		cout << "Failed to find rnr_se token. (Most likely a bad/mistyped email/passwd)\n";
		loggedin=0;
		return -2;
	}
	return 0;
}

int GoogleVoice::Init(void)
{
	hcurl=curl_easy_init(); if(!hcurl) return -1;
	curl_easy_setopt(hcurl, CURLOPT_ERRORBUFFER, errorbuf);
	curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, CurlWriter);
	curl_easy_setopt(hcurl, CURLOPT_WRITEDATA, &curlbuf);
    curl_easy_setopt(hcurl, CURLOPT_SSL_VERIFYPEER, true);
    curl_easy_setopt(hcurl, CURLOPT_SSL_VERIFYHOST, 2);

	curl_easy_setopt(hcurl, CURLOPT_HEADER, 0);
	curl_easy_setopt(hcurl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(hcurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.2.8) Gecko/20100804 Gentoo Firefox/3.6.8");

	curl_easy_setopt(hcurl, CURLOPT_COOKIEJAR, "/dev/shm/cookie.txt");
	return 0;
}

int GoogleVoice::CurlWriter(char *data, size_t size, size_t nmemb, string *buffer)
{
	if(buffer!=NULL) {buffer->append(data, size*nmemb); return size*nmemb;}
	return 0;
}

