#include "voicecommand.h"

using namespace std;
using namespace boost;

void replaceAll(string& str, const string& from, const string& to);
void changemode(int);
int  kbhit(void);

static const char *optString = "cveiqt:k:r:f:h?";

inline void ProcessVoice(FILE *cmd, VoiceCommand &vc, char *message) {
    printf("Found audio\n");
    if(!vc.quiet) {
        string command = "tts \"FILL ";
        command += vc.response;
        command += "\" 2>/dev/null 1>/dev/null";
        system(command.c_str());
    }
    cmd = popen("speech-recog.sh","r");
    fscanf(cmd,"\"%[^\"\n]\"\n",message);
    vc.ProcessMessage(message);
    fclose(cmd);
}

int main(int argc, char* argv[]) {
    VoiceCommand vc;
    //command line options
    vc.CheckCmdLineParam(argc,argv);
    
    FILE *cmd = NULL;
    char message[200];

    vc.GetConfig();
    //vc.CheckConfig();
    if(vc.quiet)
        printf("running in quiet mode\n");
    if(vc.ignoreOthers)
        printf("Not querying for answers\n");
    if(vc.edit) {
        vc.EditConfig();
    } else if(vc.continuous) {
        printf("running in continuous mode\n");
        if(vc.verify)
            printf("verifying command as well\n");
        float volume = 0.0f;
        changemode(1);
        while(vc.continuous) {
            system("arecord -D plughw:1,0 -f cd -t wav -d 2 -r 16000 /dev/shm/noise.wav 1>/dev/null 2>/dev/null");
            cmd = popen("sox /dev/shm/noise.wav -n stats -s 16 2>&1 | awk '/^Max\\ level/ {print $3}'","r");
            fscanf(cmd,"%f",&volume);
            fclose(cmd);
            if(volume > vc.thresh) {
                if(vc.verify) {
                    system("flac /dev/shm/noise.wav -f --best --sample-rate 16000 -o /dev/shm/noise.flac 1>/dev/null 2>/dev/null");
                    cmd = popen("wget -O - -o /dev/null --post-file /dev/shm/noise.flac --header=\"Content-Type: audio/x-flac; rate=16000\" http://www.google.com/speech-api/v1/recognize?lang=en | sed -e 's/[{}]/''/g'| awk -v k=\"text\" '{n=split($0,a,\",\"); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'","r");
                    if(cmd == NULL)
                        printf("ERROR\n");
                    fscanf(cmd,"\"%[^\"\n]\"\n",message);
                    fclose(cmd);
                    if(strcmp(message,vc.keyword.c_str()) == 0) {
                        message[0] = '\0'; //this will clear the first bit
                        ProcessVoice(cmd,vc,message);
                    }
                } else {
                    ProcessVoice(cmd,vc,message);
                }
                message[0] = '\0'; //this will clear the first bit
            }
            if(kbhit()) {
                if(getchar() == 27) {
                    printf("Escaping\n");
                    vc.continuous = false;
                    changemode(0);
                } else if(getchar() == 'v') {
                    if(vc.verify) {
                        printf("Turning verify off\n");
                        vc.verify = false;
                    } else {
                        printf("Turning verify on\n");
                        vc.verify = true;
                    }
                }
            }
        }
    } else {
        //system("tts \"FILL Ready?\" 2>/dev/null 1>/dev/null");
        cmd = popen("speech-recog.sh","r");
        fscanf(cmd,"\"%[^\"\n]\"\n",message);
        vc.ProcessMessage(message);
        fclose(cmd);
    }

    return 0;
}

void VoiceCommand::CheckCmdLineParam(int argc, char* argv[]) {
    //check command line configs
    int opt=0;
    continuous=false;
    verify=false;
    edit=false;
    opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'c':
                continuous = true;
                break;
            case 'v':
                verify = true;
                break;
            case 'e':
                edit = true;
                break;
            case 'i':
                ignoreOthers = true;
                break;
            case 'q':
                quiet = true;
                break;
            case 't':
                thresh = atof(optarg);
                break;
            case 'k':
                continuous = true;
                verify = true;
                keyword = string(optarg);
                break;
            case 'f':
                config_file = string(optarg);
                break;
            case 'r':
                response = string(optarg);
                break;
            case 'h': 
            case '?':
                DisplayUsage();
                break;
            default:
                break;
        }
        opt = getopt( argc, argv, optString );
    }    
}

void VoiceCommand::DisplayUsage() {
    system("man voicecommand");
    exit(0);
}

VoiceCommand::VoiceCommand() {
    hcurl = NULL;
    debug = 0;
    //Below are my default values if not changed by user
    thresh = 0.7f;
    keyword = "pi";
    response = "Yes sir?";
    quiet = false;
    char *passPath = getenv("HOME");
    if(passPath == NULL) {
        printf("Could not get $HOME\n");
        exit(-1);
    }
    config_file = string(passPath);
    config_file += "/.commands.conf";
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
    if(ignoreOthers)
        return;
    string checkit = string(message);
    if(message != NULL && !checkit.empty()) {
        printf("Attempting to answer: %s\n",message);
        Init();
        Search(message);
    } else {
        printf("No translation\n");
    }
}

void VoiceCommand::GetConfig() {
    printf("Opening config file...\n");
    ifstream file(config_file.c_str(),ios::in);
    if(!file.is_open()) {
        printf("Can't find config file!\nI'll make one.\n");
        EditConfig();
        exit(0);
    }
    string line;
    int i = 1;
    while(getline(file, line)) {
        unsigned int loc = line.find("==");
        if(line[0] == '!') {
            //This is a special config option
            //Valid options are keyword==word,continuous==#,verify==#,ignore==#,quiet==#,thresh==#f,response==word.
            string tmp = line.substr(0,8);
            if(tmp.compare("!quiet==") == 0)
                quiet = bool(atoi(line.substr(8).c_str()));
            tmp = line.substr(0,9);
            if(tmp.compare("!verify==") == 0)
                verify = bool(atoi(line.substr(9).c_str()));
            if(tmp.compare("!ignore==") == 0)
                ignoreOthers = bool(atoi(line.substr(9).c_str()));
            if(tmp.compare("!thresh==") == 0)
                thresh = atof(line.substr(9).c_str());
            tmp = line.substr(0,10);
            if(tmp.compare("!keyword==") == 0)
                keyword = line.substr(10);
            tmp = line.substr(0,11);
            if(tmp.compare("!response==") == 0)
                response = line.substr(11).c_str();
            tmp = line.substr(0,13);
            if(tmp.compare("!continuous==") == 0)
                continuous = bool(atoi(line.substr(13).c_str()));
        } else if(line[0] != '#' && loc != string::npos) {
            //This isn't a comment and is formatted properly
            string v = line.substr(0,loc);
            string c = line.substr(loc + 2);
            voice.push_back(v);
            commands.push_back(c);
        } else if(line[0] != '#') {
            printf("You have a formatting error on line %d of your config file. I'm ignoring that line\n",i);
        }
        ++i;
    }
    file.close();
}

void VoiceCommand::EditConfig() {
    printf("Editing config file...\n");
    printf("This lets you edit the config file.\nThe format is voice==command\nYou can use any character except for newlines or ==\n");
    printf("You can also put comments if the line starts with # and options if the line starts with a !\nDefault options are shown as follows:\n");
    printf("!keyword==pi,!verify==1,!continuous==1,!quiet==0,!ignore==0,!thresh=0.7,!response=Yes sir?\n");
    printf("Press any key to continue\n");
    getchar();
    string edit_command = "nano ";
    edit_command += config_file;
    system(edit_command.c_str());
}

void VoiceCommand::CheckConfig() {
    printf("Commands: \n");
    if(voice.size() > 0) {
        for(unsigned int i = 0; i < voice.size(); i++) {
            printf("%s==%s\n",voice[i].c_str(),commands[i].c_str());
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
        if(!quiet) {
            string speak = "tts \"FILL ";
            replaceAll(t,"\\n"," ");
            replaceAll(t,"|"," ");
            replaceAll(t,"\\"," ");
            speak += t;
            speak += "\" 2>/dev/null";
            system(speak.c_str());
        }
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
