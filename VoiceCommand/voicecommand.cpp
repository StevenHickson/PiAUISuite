#include "voicecommand.h"

using namespace std;
using namespace boost;

void changemode(int);
int  kbhit(void);

static const char *optString = "I:l:d:D:psb::c::v::ei::q::t:k:r:f:h?";

inline void ProcessVoice(FILE *cmd, VoiceCommand &vc, char *message) {
    printf("Found audio\n");
    vc.Speak(vc.response);
    string command = "speech-recog.sh";
    if(vc.differentHW) {
        command += " -D ";
        command += vc.recordHW;
    }
    command += " -d ";
    command += vc.duration;
    command += " -l ";
    command += vc.lang;
    cmd = popen(command.c_str(),"r");
    fscanf(cmd,"\"%[^\"\n]\"",message);
    vc.ProcessMessage(message);
    fclose(cmd);
}

inline float GetVolume(string recordHW, string com_duration, bool nullout) {
    FILE *cmd;
    float vol = 0.0f;
    string run = "arecord -D ";
    run += recordHW;
    run += " -f cd -t wav -d ";
    run += com_duration;
    run += " -r 16000 /dev/shm/noise.wav";
    if(nullout)
        run += " 1>>/dev/shm/voice.log 2>>/dev/shm/voice.log";
    system(run.c_str());
    cmd = popen("sox /dev/shm/noise.wav -n stats -s 16 2>&1 | awk '/^Max\\ level/ {print $3}'","r");
    fscanf(cmd,"%f",&vol);
    fclose(cmd);
    return vol;
}

int main(int argc, char* argv[]) {
    VoiceCommand vc;
    //this is a really crude and terrible hack.
    //It makes it so that the config file doesn't overwrite the command line options
    //And it allows the config file to be set to something random
    system("echo \"\" > /dev/shm/voice.log"); //lazily clear out the log file
    vc.CheckConfigParam(argc,argv);
    
    FILE *cmd = NULL;
    char message[200];
    message[0] = '\0';

    vc.GetConfig();
    //command line options after the config options
    vc.CheckCmdLineParam(argc,argv);
    //vc.CheckConfig();
    if(!vc.pid_file.empty()) {
        FILE *out;
        out = fopen(vc.pid_file.c_str(),"w");
        if(out == NULL)
            printf("Can not write to pid file: %s\n",vc.pid_file.c_str());
        else {
            fprintf(out,"%d",getpid());
            fclose(out);
            printf("Wrote pid file\n");
        }
    }
    if(vc.quiet)
        fprintf(stderr,"running in quiet mode\n");
    if(vc.ignoreOthers)
        fprintf(stderr,"Not querying for answers\n");
    if(vc.edit) {
        vc.EditConfig();
    } else if(vc.continuous && vc.forced_input.empty()) {
        fprintf(stderr,"running in continuous mode\n");
        if(vc.verify)
            fprintf(stderr,"verifying command as well\n");
        fprintf(stderr,"keyword duration is %s and duration is %s\n",vc.command_duration.c_str(),vc.duration.c_str());
        float volume = 0.0f;
        changemode(1);
        string cont_com = "curl -X POST --data-binary @/dev/shm/noise.flac --user-agent 'Mozilla/5.0' --header 'Content-Type: audio/x-flac; rate=16000;' 'https://www.google.com/speech-api/v2/recognize?output=json&lang=" + vc.lang + "&key=AIzaSyBOti4mM-6x9WDnZIjIeyEU21OpBXqWBgw&client=Mozilla/5.0' | sed -e 's/[{}]/''/g' | awk -F\":\" '{print $4}' | awk -F\",\" '{print $1}' | tr -d '\\n'";

        while(vc.continuous) {
            volume = GetVolume(vc.recordHW, vc.command_duration, true);
            if(volume > vc.thresh) {
                //printf("Found volume %f above thresh %f\n",volume,vc.thresh);
                if(vc.verify) {
                    system("flac /dev/shm/noise.wav -f --best --sample-rate 16000 -o /dev/shm/noise.flac 1>>/dev/shm/voice.log 2>>/dev/shm/voice.log");
                    cmd = popen(cont_com.c_str(),"r");
                    if(cmd == NULL)
                        printf("ERROR\n");
                    fscanf(cmd,"\"%[^\"\n]\"",message);
                    fclose(cmd);
                    system("rm -fr /dev/shm/noise.*");
                    //printf("message: %s, keyword: %s\n", message, vc.keyword.c_str());
                    if(iequals(message,vc.keyword.c_str())) {
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
        if(vc.forced_input.empty()) {
            string command = "speech-recog.sh";
            if(vc.differentHW) {
                command += " -D ";
                command += vc.recordHW;
            }
            command += " -d ";
            command += vc.duration;
            command += " -l ";
            command += vc.lang;
            cmd = popen(command.c_str(),"r");
            fscanf(cmd,"\"%[^\"\n]\"",message);
            vc.ProcessMessage(message);
            fclose(cmd);
        } else {
            vc.ProcessMessage(vc.forced_input.c_str());
        }
    }

    return 0;
}

inline void VoiceCommand::CheckConfigParam(int argc, char* argv[]) {
    //check to see if they set a different config file
    int opt=0;
    opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'f':
                config_file = string(optarg);
                optind=1;
                return;
            default:
                break;
        }
        opt = getopt( argc, argv, optString );
    }
    optind=1;
}

inline void VoiceCommand::CheckCmdLineParam(int argc, char* argv[]) {
    //check command line configs
    int opt=0;
    opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'b':
                if(optarg && !bool(atoi(optarg)))
                    filler = "\"";
                else
                    filler = "\"FILLER FILL ";
                break;
            case 'c':
                if(optarg && !bool(atoi(optarg)))
                    continuous = false;
                else
                    continuous = true;
                break;
            case 'd':
                duration = string(optarg);
                break;
            case 'D':
                recordHW = string(optarg);
                differentHW = true;
            case 'p':
                passthrough = true;
            case 'I':
                forced_input = string(optarg);
            case 'v':
                if(optarg && !bool(atoi(optarg)))
                    verify = false;
                else
                    verify = true;
                break;
            case 'e':
                edit = true;
                break;
            case 'i':
                if(optarg && !bool(atoi(optarg)))
                    ignoreOthers = false;
                else
                    ignoreOthers = true;
                break;
            case 'q':
                if(optarg && !bool(atoi(optarg)))
                    quiet = false;
                else
                    quiet = true;
                break;
            case 'l':
                command_duration = string(optarg);
                break;
            case 's':
                Setup();
                exit(0);
            case 't':
                thresh = atof(optarg);
                break;
            case 'k':
                continuous = true;
                verify = true;
                keyword = string(optarg);
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
    //behold my laziness
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
    improper = "Received improper command";
    lang="en";
    quiet = false;
    filler = "\"FILLER FILL ";
    continuous = false;
    verify = false;
    edit = false;
    ignoreOthers = false;
    differentHW = false;
    passthrough = false;
    recordHW = "plughw:1,0";
    pid_file.clear();
    api.clear();
    forced_input.clear();
    duration = DURATION_DEFAULT;
    command_duration = COM_DURATION_DEFAULT;
    maxResponse = -1;
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


inline void VoiceCommand::ProcessMessage(const char* message) {
    unsigned int i = 0, loc = 0;
    string tmp = message;
    string sTmp = message;
    to_upper(sTmp);
    while(i < voice.size()) {
        loc = sTmp.find(voice[i]);
        if(loc == 0) {
            tmp = commands[i];
            loc = tmp.find("...");
            if(loc != string::npos) {
                //Found ... Initiating special options
                string newcommand = tmp.substr(0,loc-1);
                string options = message;
                newcommand += options.substr(voice[i].length());
                if(passthrough)
                    printf("%s",newcommand.c_str());
                else {
                    printf("command: %s\n",newcommand.c_str());
                    system(newcommand.c_str());
                }
            } else {
                if(passthrough)
                    printf("%s",tmp.c_str());
                else {
                    printf("command: %s\n",tmp.c_str());
                    system(tmp.c_str());
                }
            }
            return;
        } else if( voice[i][0] == '~' ) {
	    // see whether the voice keyword is *anywhere* in the message
	    string v = voice[i].substr(1, string::npos);
	    loc = sTmp.find(v);
	    //printf("v: %s\tloc: %d\tsTmp: %s\n",v.c_str(),loc,sTmp.c_str());
	    if( loc != string::npos && loc != -1) {				
	        // if it does, return
                if(passthrough)
                    printf("%s",commands[i].c_str());
                else {
	            printf("command: %s\n",commands[i].c_str());
		    system(commands[i].c_str());
                }
		return;
	    }				
	} else {
            regex rexp("\\$(\\d+)"); cmatch m;
            if(regex_search(voice[i].c_str(), m, rexp)) {
                //Found $ Initiating special options
                int num_var = m.size() + 1;
                //fprintf(stderr, "Found # %d $s, initiating special option\n", num_var);
                string match = voice[i];
                for(int j = 1; j <= num_var; j++) {
                    stringstream replace;
                    replace << "$";
                    replace << j;
                    replace_all(match,replace.str(),"([^\t\n]+?)");
                }
                regex rexp2(match); cmatch n;
                //this line is the bug somehow (I think)
                if(regex_search(sTmp.c_str(), n, rexp2)) {
                    string run = commands[i];
                    for(int j = 0; j <= num_var; j++) {
                        //fprintf(stderr, "Found %s, initiating special option stage2\n",string(n[j]).c_str());
                        stringstream replace;
                        replace << "$";
                        replace << j;
                        replace_all(run, replace.str(), string(n[j]));
                    }
                    if(passthrough)
                        printf("%s",run.c_str());
                    else {
                        printf("command: %s\n",run.c_str());
                        system(run.c_str());
                    }
                    return;
                }
            }
        }
        ++i;
    }
    string improper_tmp = improper + ": " + message + "\n";
    if(ignoreOthers) {
        fprintf(stderr,improper_tmp.c_str());
        Speak(improper);
        return;
    }
    string checkit = string(message);
    if(message != NULL && !checkit.empty()) {
        fprintf(stderr,"Attempting to answer: %s\n",message);
        Init();
        Search(message);
    } else if(!passthrough) {
        printf("No translation\n");
        Speak("No translation");
    } 
}

void VoiceCommand::GetConfig() {
    fprintf(stderr,"Opening config file...\n");
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
            //Valid options are keyword==word,continuous==#,verify==#,ignore==#,quiet==#,thresh==#f,response==word improper==word.
            string tmp = line.substr(0,6);
            if(tmp.compare("!api==") == 0)
                api = line.substr(6);
            tmp = line.substr(0,7);
            if(tmp.compare("!lang==") == 0)
                lang = line.substr(7);
            tmp = line.substr(0,8);
            if(tmp.compare("!quiet==") == 0)
                quiet = bool(atoi(line.substr(8).c_str()));
            tmp = line.substr(0,9);
            if(tmp.compare("!verify==") == 0)
                verify = bool(atoi(line.substr(9).c_str()));
            if(tmp.compare("!ignore==") == 0)
                ignoreOthers = bool(atoi(line.substr(9).c_str()));
            if(tmp.compare("!thresh==") == 0)
                thresh = atof(line.substr(9).c_str());
            if(tmp.compare("!filler==") == 0) {
                filler = line.substr(9);
                if(filler.compare("1") == 0)
                    filler = "\"FILLER FILL ";
                else if(filler.compare("0") == 0)
                    filler = "\"";
                else
                    filler = "\"" + filler;
            }
            tmp = line.substr(0,10);
            if(tmp.compare("!keyword==") == 0)
                keyword = line.substr(10);
            if(tmp.compare("!com_dur==") == 0)
                command_duration = line.substr(10);
            if(tmp.compare("!pidfile==") == 0)
                pid_file = line.substr(10);
            tmp = line.substr(0,11);
            if(tmp.compare("!response==") == 0)
                response = line.substr(11);
            if(tmp.compare("!improper==") == 0)
                improper = line.substr(11);
            if(tmp.compare("!hardware==") == 0) {
                recordHW = line.substr(11);
                differentHW = true;
            }
            if(tmp.compare("!language==") == 0)
                lang = line.substr(11);
            if(tmp.compare("!duration==") == 0)
                duration = line.substr(11);
            tmp = line.substr(0,13);
            if(tmp.compare("!continuous==") == 0)
                continuous = bool(atoi(line.substr(13).c_str()));
             tmp = line.substr(0,14);
             if(tmp.compare("!maxResponse==") == 0) {
                maxResponse = atoi(line.substr(14).c_str());
                if (maxResponse < 1)
                    maxResponse = -1;
             }
        } else if(loc < 500 && loc != string::npos && line[0] != '#') {
            //This isn't a comment and is formatted properly
            string v = line.substr(0,loc);
            string c = line.substr(loc + 2);
            to_upper(v);
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
    printf("Editing config file...\n"
    "This lets you edit the config file.\nThe format is voice==command\nYou can use any character except for newlines or ==\n"
    "If the voice starts with ~, the program looks for the keyword anywhere. Ex: ~weather would pick up on weather or what's the weather\n"
    "You can use ... at the end of the command to specify that everything after the given keyword should be options to the command.\n"
    "Ex: play==playvideo ...\nThis means that if you say \"play Futurama\", it will run the command playvideo Futurama\n"
    "You can use $# (where # is any number 1 to 9) to represent a variable. These should go in order from 1 to 9\n"
    "Ex: $1 season $2 episode $3==playvideo -s $2 -e $3 $1\n"
    "This means if you say game of thrones season 1 episode 2, it will run playvideo with the -s flag as 1, the -e flag as 2, and the main argument as game of thrones, i.e. playvideo -s 1 -e 2 game of thrones\n"
    "Because of these options, it is important that the arguments range from most strict to least strict."
    "This means that ~ arguments should probably be at the end.\n"
    "arguments with multiple variables like the play $1 season $2 episode $3 example should be before ones like play... because it will pick the first match\n"
    "You can also put comments if the line starts with # and options if the line starts with a !\nDefault options are shown as follows:\n"
    "!keyword==pi,!verify==1,!continuous==1,!quiet==0,!ignore==0,!thresh=0.7,!response=Yes sir?, !improper=Received improper command:,!duration==3,!com_dur==2,!filler==FILLER FILL,!api==BLANK,!maxResponse==-1,"
    "!lang==en,!hardware==plughw:1,0\n"
    "Press any key to continue\n");
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

int VoiceCommand::Speak(string message) {
    if (quiet) {
	    return 0;
    }

    string command = "tts -l " + lang + " " + filler;

    command += message;
    command += "\" 2>>/dev/shm/voice.log 1>>/dev/shm/voice.log";
    system(command.c_str());

    return 0;
}

string from_html_entities(string src)
{
  struct lookup_s
  {
    const string from;
    const string to;
  };

  static const struct lookup_s conv[] =
  {
    {"&quot;", "\""},
    {"&#34;", "\""},
    {"&#034;", "\""},
    {"&amp;", "&"},
    {"&#38;", "&"},
    {"&#038;", "&"},
    {"&apos;", "'"},
    {"&#39;", "'"},
    {"&#039;", "'"},
    {"&lt;", "<"},
    {"&#60;", "<"},
    {"&#060;", "<"},
    {"&gt;", ">"},
    {"&#62;", ">"},
    {"&#062;", ">"},
    {"", ""}
  };

  int i;

  for (i = 0; !conv[i].from.empty(); ++i) {
    replace_all(src, conv[i].from, conv[i].to);
  }

  return (src);
}

int VoiceCommand::Search(const char* search) {
    if(!init)
        Init();
    //technically this should never happen now.
    if (!hcurl) {
        cout << "hcurl is NULL.  Did you forget to call Init()?\n";
        return -1;
    }

    if(api.empty()) {
        string link = "https://www.wolframalpha.com/input/?i=";
        link += search;
        replace_all(link, string(" "), string("%20"));
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
            replace_all(t,"\\n",". \n");
            printf("%s\n", t.c_str());
            Speak(t);
            return 0;
        } else {
            printf("Could not find answer. Try again.\n");
            Speak("Could not find answer. Try again");
            return -1;
        }
    } else {
        string link = "http://api.wolframalpha.com/v2/query?appid=";
        link += api;
        link += "&reinsterpret=true&translation=true&format=plaintext&input=";
        char *curllink = curl_easy_escape(hcurl, search, 0);
        link += curllink;
        curl_free(curllink);
        //printf("link: %s\n",link.c_str());
        curl_easy_setopt(hcurl, CURLOPT_URL, link.c_str());
        curlbuf.clear();
        cr = curl_easy_perform(hcurl);
        if (cr != CURLE_OK) {
            cout << "curl() error on getting link: " << errorbuf << endl;
            return -3;
        }

        if (debug & 2) cout << "\nLink curlbuf: [" << curlbuf << "]\n";

        if (1) {
	        using boost::property_tree::ptree;
	        ptree pt;
	        stringstream ss;
	        ss << string(curlbuf);
	        read_xml(ss, pt);
	        string result = "";
	        string plain;
	        int limit = maxResponse;
	        BOOST_FOREACH( ptree::value_type const& v, pt.get_child("queryresult") ) {
	            if (v.first == "pod") {
		            string title = v.second.get<string>("<xmlattr>.title");
		            BOOST_FOREACH( ptree::value_type const& v2, v.second.get_child("subpod") ) {
		                if (limit != 0 && v2.first == "plaintext") {
			                plain = v2.second.data();
			                if (!plain.empty()) {
			                    if (title == "Input interpretation" || title == "Input") {
				                    printf("%s : %s\n", title.c_str(), plain.c_str());
			                    } else {
				                    if (title != "Response") {
				                        result += title + " : ";
		 		                    }
				                    result += plain + "\n";
				                    limit--;
			                    }
			                }
		                }
		            }
	            }
            }	
	        if (result.empty()) {
	            BOOST_FOREACH( ptree::value_type const& v, pt.get_child("queryresult") ) {
		            if (v.first == "didyoumeans") {
		                result = v.second.get<string>("didyoumean");
		                printf("No luck, will try with %s\n", result.c_str());
		                return Search(result.c_str());
		            }
                }
	            printf("Could not find answer. Try again.\n");
                Speak("Could not find answer. Try again");
	        } else {
                result = from_html_entities(result);
                printf("%s\n", result.c_str());
                Speak(result);
                return 0;
	        }
        } else {
            regex rexp("<plaintext>([^<].+?)</plaintext>");
            smatch m;
            string::const_iterator endbuf = curlbuf.end();
            if (regex_search(curlbuf, m, rexp) 
        && regex_search(m[1].second, endbuf, m, rexp)) {
                //char * curlrep = curl_easy_unescape(hcurl, m.str(1).c_str(), 0, NULL);
                //string t = string(curlrep);
                //curl_free(curlrep);
	            string t = string(m.str(1));
	            t = from_html_entities(t);
                printf("%s\n", t.c_str());
	            Speak(t);
                return 0;
            } else {
	            regex rexp2("<didyoumean [^>]*?>(.+?)</didyoumean>");
    	        cmatch m2;
    	        if (regex_search(curlbuf.c_str(), m2, rexp2)) {
	                printf("No luck, will try with %s\n", m2.str(1).c_str());
	                return Search(m2.str(1).c_str());
	            }

                printf("Could not find answer. Try again.\n");
                Speak("Could not find answer. Try again");
                return -1;
            }
        }
    }
    return 0;
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

void VoiceCommand::Setup() {
    //Function in order to detect your default options.
    //It will then set these automatically in the config file.
    bool change=false;
    char buffer[100];
    string write = "";
    printf("Do you want to permanently set the continuous flag so that it always runs continuously? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        continuous = true;
        write += "!continuous==1\n";
    }
    printf("Do you want to permanently set the verify flag so that it always verifies the keyword? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        verify = true;
        write += "!verify==1\n";
    }
    printf("Do you want to permanently set the ignore flag so that it never looks for answers outside the config file? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        ignoreOthers = true;
        write += "!ignore==1\n";
    }
    printf("Do you want to permanently set the quiet flag so that it never uses audio out to speak? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        quiet = true;
        write += "!quiet==1\n";
    }
    printf("Do you want to permanently change the default duration of the speech recognition (3 seconds)? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        printf("Type the number of seconds you want it to run: ex 3\n");
        int num;
        scanf("%d", &num);
        write += "!duration==";
        stringstream tmp;
        tmp << num;
        duration = tmp.str();
        write += duration;
        write += "\n";
    }
    printf("Do you want to permanently change the default command duration of the speech recognition (2 seconds)? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        printf("Type the number of seconds you want it to run: ex 2\n");
        int num;
        scanf("%d", &num);
        write += "!com_dur==";
        stringstream tmp;
        tmp << num;
        command_duration = tmp.str();
        write += command_duration;
        write += "\n";
    }

    //Now we will check some more options and check the TTS and speech recognition
    printf("Do you want to set up and check the text to speech options? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        printf("First I'm going to say something and see if you hear it\n");
        system("tts \"FILLER FILL This program was created by Steven Hickson\"");
        printf("Did you hear anything? (y/n)\n");
        scanf("%s",buffer);
        if(buffer[0] == 'n') {
            printf("Something went wrong and you should run the install script and install the dependecies as well\n");
            printf("You might also want to change some alsa options using alsamixer and alsa force-reload\n");
            exit(-1);
        }
        printf("\nIf you heard the word FILL (or FILLER or FILLER FILL) at the beginning of the sentence, the filler flag should be set to 0 or be blank\n");
        printf("Do you want me to permanently set the filler flag to 0? (y/n)\n");
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            filler = "\"";
            write += "!filler==0\n";
        }
        string cmd;
        printf("\nThe default response of the system after it finds the keyword is \"Yes Sir?\"\n");
        printf("Do you want to change the response? (y/n)\n");
        change = false;
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            change = true;
            while(change) {
                printf("Type the phrase you want as the response:\n");
                scanf("%s",buffer);
                cmd = "tts " + filler;
                cmd += string(buffer);
                cmd += "\"";
                system(cmd.c_str());
                response = string(buffer);
                printf("Did that sound correct? (y/n)\n");
                scanf("%s",buffer);
                if(buffer[0] == 'y')
                    change = false;
            }
            write += "!response==";
            write += response;
            write += "\n";
        }
        printf("\nThe default response of the system after it receives an unknown command is \"Received improper command:\"\n");
        printf("Do you want to change the response? (y/n)\n");
        change = false;
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            change = true;
            while(change) {
                printf("Type the phrase you want as the 'command not found' response:\n");
                scanf("%s",buffer);
                cmd = "tts " + filler;
                cmd += string(buffer);
                cmd += "\"";
                system(cmd.c_str());
                improper = string(buffer);
                printf("Did that sound correct? (y/n)\n");
                scanf("%s",buffer);
                if(buffer[0] == 'y')
                    change = false;
            }
            write += "!improper==";
            write += improper;
            write += "\n";
        }
    }
    
    //Now we will check some more options and check the TTS and speech recognition
    printf("Do you want to set up and check the speech recognition options? (y/n)\n");
    scanf("%s",buffer);
    if(buffer[0] == 'y') {
        printf("First I'm going to make sure you have the correct hardware device\n");
        FILE *cmd;
        int card = -1,device = -1;
        cmd = popen("arecord -l | awk '/^card [0-9]/ {print $2}'","r");
        fscanf(cmd, "%d:",&card);
        cmd = popen("arecord -l | grep -o 'device [0-9]:' | grep -o  '[0-9]:'","r");
        fscanf(cmd, "%d:",&device);
        if(card == -1 || device == -1) {
            printf("I couldn't find a hardware device. You don't have a valid microphone\n");
            exit(-1);
        } else if(card != 1 || device != 0) {
            printf("I detected that you have a different audio card then I located, would you like me to fix that in the config file? (y/n)\n");
            scanf("%s",buffer);
            if(buffer[0] == 'y') {
                stringstream tmp;
                tmp << "plughw:";
                tmp << card;
                tmp << ",";
                tmp << device;
                recordHW = tmp.str();
                differentHW = true;
                write += "!hardware==";
                write += recordHW;
                write += "\n";
            }
        } else 
            printf("Everything seems right with the hardware config\n");
        printf("\nWould you like me to try to get the proper audio threshold? (y/n)\n");
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            float low, high;
            printf("I'm going to record you once while you are silent and then once while you say the command in order to determine the threshold\n");
            printf("Getting ready for silent recording, just don't say anything while this is happening, press any key when ready\n");
            getchar();
            getchar(); //Needed it twice here for whatever reason
            low = GetVolume(recordHW, command_duration, false);
            printf("Getting ready for command recording, try saying the command while this is happening, press any key when ready\n");
            getchar();
            high = GetVolume(recordHW, command_duration, false);
            float tmp = (high - low) * 0.75f + low;
            if(tmp != thresh) {
                printf("I detected that your default thresh: %f is different than the thresh I detected that you should use: %f\n",thresh,tmp);
                printf("Should I set that in the config file? (y/n)\n");
                scanf("%s",buffer);
                if(buffer[0] == 'y') {
                    thresh = tmp;
                    stringstream convert;
                    convert << thresh;
                    write += "!thresh==";
                    write += convert.str();
                    write += "\n";
                }
            }
        }
        printf("\nThe default keyword of the system is \"pi\"\n");
        printf("Do you want to change the keyword? (y/n)\n");
        change = false;
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            change = true;
            FILE *cmd;
            while(change) {
                printf("Type the phrase you want as the keyword:\n");
                scanf("%s",buffer);
                keyword = string(buffer);
                char message[200];
                printf("Now say that keyword\n");
                string command = "speech-recog.sh";
                if(differentHW) {
                    command += " -D ";
                    command += recordHW;
                }
                command += " -d ";
                command += duration;
                cmd = popen(command.c_str(),"r");
                fscanf(cmd,"\"%[^\"\n]\"\n",message); 
                if(iequals(message,keyword.c_str()))
                    printf("I got %s, which was a perfect match!\n",message);
                else
                    printf("I got %s, which was different than what you typed: %s\n",message,keyword.c_str());
                printf("Did that seem correct? (y/n)\n");
                scanf("%s",buffer);
                if(buffer[0] == 'y')
                    change = false;
            }
            write += "!keyword==";
            write += keyword;
            write += "\n";
        }
    }

    //Now we will write everything to the config file
    string tmp = "echo \"";
    tmp += write;
    tmp += "\" >> ";
    tmp += config_file;
    //I am doing it this way because I'm lazy
    system(tmp.c_str());
    printf("Done setting everything up!\n");
}
