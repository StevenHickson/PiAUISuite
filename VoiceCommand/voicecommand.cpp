#include "voicecommand.h"

using namespace std;
using namespace boost;

void replaceAll(string& str, const string& from, const string& to);
void changemode(int);
int  kbhit(void);

int main(int argc, char* argv[]) {
    FILE *cmd = NULL;
    char message[200];

    VoiceCommand vc;
    vc.GetConfig();
    //vc.CheckConfig();
    bool continuous = false;
    if(argc == 2 && strcmp(argv[1],"-c") == 0) {
        printf("running in continuous mode\n");
        continuous = true;
        float volume = 0.0f;
        changemode(1);
        while(continuous) {
            system("arecord -D plughw:1,0 -f cd -t wav -d 2 -r 16000 /dev/shm/noise.wav 1>/dev/null 2>/dev/null");
            cmd = popen("sox /dev/shm/noise.wav -n stats -s 16 2>&1 | awk '/^Max\\ level/ {print $3}'","r");
            fscanf(cmd,"%f",&volume);
            fclose(cmd);
            if(volume > 0.7f) {
                /*system("flac /dev/shm/noise.wav -f --best --sample-rate 16000 -o /dev/shm/noise.flac 1>/dev/null 2>/dev/null");
                cmd = popen("wget -O - -o /dev/null --post-file /dev/shm/noise.flac --header=\"Content-Type: audio/x-flac; rate=16000\" http://www.google.com/speech-api/v1/recognize?lang=en | sed -e 's/[{}]/''/g'| awk -v k=\"text\" '{n=split($0,a,\",\"); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'","r");
                if(cmd == NULL)
                    printf("ERROR\n");
                fscanf(cmd,"\"%[^\"\n]\"\n",message);
                fclose(cmd);
                if(strcmp(message,"pi") == 0) {*/
                    printf("Found audio\n");
                    system("tts \"FILLER FILL Ready?\" 2>/dev/null 1>/dev/null");
                    cmd = popen("speech-recog.sh","r");
                    fscanf(cmd,"\"%[^\"\n]\"\n",message);
                    vc.ProcessMessage(message);
                    fclose(cmd);
                //}
                message[0] = '\0'; //this will clear the first bit
            }
            if(kbhit()) {
                if(getchar() == 27) {
                    printf("Escaping\n");
                    continuous = false;
                    changemode(0);
                }
            }
        }
    } else if(argc == 2 && strcmp(argv[1],"-e") == 0) {
        //Edit the config file
        vc.EditConfig();
    } else {
        //system("tts \"FILLER FILL Ready?\" 2>/dev/null 1>/dev/null");
        cmd = popen("speech-recog.sh","r");
        fscanf(cmd,"\"%[^\"\n]\"\n",message);
        vc.ProcessMessage(message);
        fclose(cmd);
    }

    return 0;
}

VoiceCommand::VoiceCommand() {
    hcurl = NULL;
    debug = 0;
}

VoiceCommand::~VoiceCommand() {
    if (hcurl) curl_easy_cleanup(hcurl);
}


inline void VoiceCommand::ProcessMessage(char* message) {
    unsigned int i = 0, loc = 0;
    string tmp = message;
    while(i < voice.size()) {
        loc = tmp.find(voice[i]);
        if(loc == 0) {
            tmp = commands[i];
            loc = tmp.find("...");
            if(loc != string::npos) {
                //Found ... Initiating special options
                string newcommand = tmp.substr(0,loc-1);
                string options = message;
                newcommand += options.substr(voice[i].length());
                printf("command: %s\n",newcommand.c_str());
                system(newcommand.c_str());
            } else {
                printf("command: %s\n",tmp.c_str());
                system(tmp.c_str());
            }
            return;
        } else {
            regex rexp("\\$(\\d+)"); cmatch m;
            if(regex_search(voice[i].c_str(), m, rexp)) {
                //Found $ Initiating special options
                int num_var = m.size() + 1;
                string match = voice[i];
                for(int j = 1; j <= num_var; j++) {
                    stringstream replace;
                    replace << "$";
                    replace << j;
                    replaceAll(match,replace.str(),"([^\t\n]+?)");
                }
                regex rexp2(match); cmatch n;
                if(regex_search(message, n, rexp2)) {
                    string run = commands[i];
                    for(int j = 0; j <= num_var; j++) {
                        stringstream replace;
                        replace << "$";
                        replace << j;
                        replaceAll(run, replace.str(), string(n[j]));
                    }
                    printf("command: %s\n",run.c_str());
                    system(run.c_str());
                    return;
                }
            }
        }
        ++i;
    }
    printf("Attempting to answer: %s\n",message);
    Init();
    Search(message);
}

void VoiceCommand::GetConfig() {
    printf("Opening config file...\n");
    FILE* fp;
    char *passPath;
    passPath = getenv("HOME");
    if(passPath == NULL) {
        printf("Could not get $HOME\n");
        exit(-1);
    }
    string path = string(passPath);
    path += "/.commands.conf";
    fp = fopen(path.c_str(),"r");
    if(fp == NULL) {
        printf("Can't find config file commands.conf!\n");
        exit(-1);
    }
    char v[200],c[200];
    while(fscanf(fp,"%[^=\n]=%[^=\n]\n",v,c) != EOF) {
        voice.push_back(v);
        commands.push_back(c);
    }
    fclose(fp);
}

void VoiceCommand::EditConfig() {
    printf("Editing config file...\n");
    char *passPath;
    passPath = getenv("HOME");
    if(passPath == NULL) {
        printf("Could not get $HOME\n");
        exit(-1);
    }
    string path = string(passPath);
    printf("This lets you edit the config file.\nThe format is voice=command\nYou can use any character except for newlines or =\n");
    printf("Press any key to continue\n");
    getchar();
    string edit_command = "nano ";
    edit_command += string(passPath);
    edit_command += "/.commands.conf";
    system(edit_command.c_str());
}

void VoiceCommand::CheckConfig() {
    printf("Commands: \n");
    if(voice.size() > 0) {
        for(unsigned int i = 0; i < voice.size(); i++) {
            printf("%s=%s\n",voice[i].c_str(),commands[i].c_str());
        }
    }
}

int VoiceCommand::Init(void) {
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

int VoiceCommand::CurlWriter(char *data, size_t size, size_t nmemb, string *buffer) {
    if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        return size*nmemb;
    }
    return 0;
}

int VoiceCommand::Search(const char* search) {
    if(!init)
        Init();
    //technically this should never happen now.
    if (!hcurl) {
        cout << "hcurl is NULL.  Did you forget to call Init()?\n";
        return -1;
    }

    string link = "https://www.wolframalpha.com/input/?i=";
    link += search;
    replaceAll(link, string(" "), string("%20"));
    //printf("link: %s\n",link.c_str());
    curl_easy_setopt(hcurl, CURLOPT_URL, link.c_str());
    curlbuf.clear();
    cr = curl_easy_perform(hcurl);
    if (cr != CURLE_OK) {
        cout << "curl() error on getting link: " << errorbuf << endl;
        return -3;
    }

    if (debug & 2) cout << "\nLink curlbuf: [" << curlbuf << "]\n";

    regex rexp("0200\\.push\\( \\{\"stringified\": \"(.+?)\",\"");
    cmatch m;
    if (regex_search(curlbuf.c_str(), m, rexp)) {
        string t = string(m[1]);
        printf("%s\n", t.c_str());
        string speak = "tts \"FILLER FILL ";
        replaceAll(t,"\\n"," ");
        replaceAll(t,"|"," ");
        replaceAll(t,"\\"," ");
        speak += t;
        speak += "\" 2>/dev/null";
        system(speak.c_str());
        return 0;
    } else {
        cout << "Could not find answer. Try again.\n";
        return -1;
    }

    return 0;
}

//little helper function
void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void changemode(int dir)
{
  static struct termios oldt, newt;
 
  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
 
int kbhit (void)
{
  struct timeval tv;
  fd_set rdfs;
 
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
 
}
