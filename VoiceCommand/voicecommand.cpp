#include "voicecommand.h"

using namespace std;
using namespace boost;

void replaceAll(string& str, const string& from, const string& to);
void changemode(int);
int  kbhit(void);

static const char *optString = "l:d:D:sbcveiqt:k:r:f:h?";

inline void ProcessVoice(FILE *cmd, VoiceCommand &vc, char *message) {
    printf("Found audio\n");
    if(!vc.quiet) {
        string command;
        if(vc.filler)
            command = "tts \"FILL ";
        else
            command = "tts-nofill \"";
        command += vc.response;
        command += "\" 2>/dev/null 1>/dev/null";
        system(command.c_str());
    }
    string command = "speech-recog.sh";
    if(vc.differentHW) {
        command += " -D ";
        command += vc.recordHW;
    }
    command += " -d ";
    command += vc.duration;
    cmd = popen(command.c_str(),"r");
    fscanf(cmd,"\"%[^\"\n]\"\n",message);
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
        run += " 1>/dev/null 2>/dev/null";
    system(run.c_str());
    cmd = popen("sox /dev/shm/noise.wav -n stats -s 16 2>&1 | awk '/^Max\\ level/ {print $3}'","r");
    fscanf(cmd,"%f",&vol);
    fclose(cmd);
    return vol;
}

int main(int argc, char* argv[]) {
    VoiceCommand vc;
    //command line options
    vc.CheckCmdLineParam(argc,argv);
    
    FILE *cmd = NULL;
    char message[200];
    message[0] = '\0';

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
        printf("keyword duration is %s and duration is %s\n",vc.command_duration.c_str(),vc.duration.c_str());
        float volume = 0.0f;
        changemode(1);
        while(vc.continuous) {
            volume = GetVolume(vc.recordHW, vc.command_duration, true);
            if(volume > vc.thresh) {
                //printf("Found volume %f above thresh %f\n",volume,vc.thresh);
                if(vc.verify) {
                    system("flac /dev/shm/noise.wav -f --best --sample-rate 16000 -o /dev/shm/noise.flac 1>/dev/null 2>/dev/null");
                    cmd = popen("wget -O - -o /dev/null --post-file /dev/shm/noise.flac --header=\"Content-Type: audio/x-flac; rate=16000\" http://www.google.com/speech-api/v1/recognize?lang=en | sed -e 's/[{}]/''/g'| awk -v k=\"text\" '{n=split($0,a,\",\"); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'","r");
                    if(cmd == NULL)
                        printf("ERROR\n");
                    fscanf(cmd,"\"%[^\"\n]\"\n",message);
                    fclose(cmd);
                    //system("rm -fr /dev/shm/noise.*");
                    //printf("message: %s, keyword: %s\n", message, vc.keyword.c_str());
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
        string command = "speech-recog.sh";
        if(vc.differentHW) {
            command += " -D ";
            command += vc.recordHW;
        }
        command += " -d ";
        command += vc.duration;
        cmd = popen(command.c_str(),"r");
        fscanf(cmd,"\"%[^\"\n]\"\n",message);
        vc.ProcessMessage(message);
        fclose(cmd);
    }

    return 0;
}

void VoiceCommand::CheckCmdLineParam(int argc, char* argv[]) {
    //check command line configs
    int opt=0;
    opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'b':
                filler = false;
                break;
            case 'c':
                continuous = true;
                break;
            case 'd':
                duration = string(optarg);
                break;
            case 'D':
                recordHW = string(optarg);
                differentHW = true;
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
    filler = true;
    continuous = false;
    verify = false;
    edit = false;
    ignoreOthers = false;
    differentHW = false;
    recordHW = "plughw:1,0";
    duration = DURATION_DEFAULT;
    command_duration = COM_DURATION_DEFAULT;
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
        } else if( voice[i][0] == '~' ) {
			// see whether the voice keyword is *anywhere* in the message
			string v = voice[i].substr(1, string::npos);
			loc = tmp.find(v);
			if( loc != string::npos ) {				
				// if it does, return
				printf("command: %s\n",commands[i].c_str());
				system(commands[i].c_str());					
				return;
			}				
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
    if(ignoreOthers) {
        printf("Received improper command: %s\n",message);
        return;
    }
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
            if(tmp.compare("!filler==") == 0)
                filler = atof(line.substr(9).c_str());
            tmp = line.substr(0,10);
            if(tmp.compare("!keyword==") == 0)
                keyword = line.substr(10);
            if(tmp.compare("!com_dur==") == 0)
                command_duration = line.substr(10);
            tmp = line.substr(0,11);
            if(tmp.compare("!response==") == 0)
                response = line.substr(11);
            if(tmp.compare("!hardware==") == 0) {
                recordHW = line.substr(11);
                differentHW = true;
            }
            if(tmp.compare("!duration==") == 0)
                duration = line.substr(11);
            tmp = line.substr(0,13);
            if(tmp.compare("!continuous==") == 0)
                continuous = bool(atoi(line.substr(13).c_str()));
        } else if(loc < 500 && loc != string::npos && line[0] != '#') {
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
    printf("Editing config file...\n"
    "This lets you edit the config file.\nThe format is voice==command\nYou can use any character except for newlines or ==\n"
    "If the voice starts with ~, the program looks for the keyword anywhere. Ex: ~weather would pick up on weather or what's the weather\n"
    "You can use ... at the end of the command to specify that everything after the given keyword should be options to the command.\n"
    "Ex: play==playvideo ...\nThis means that if you say \"play Futurama\", it will run the command playvideo Futurama\n"
    "You can use $# (where # is any number 1 to 9) to represent a variable. These should go in order from 1 to 9\n"
    "Ex: play $1 season $2 episode $3==playvideo -s $2 -e $3 $1\n"
    "This means if you say play game of thrones season 1 episode 2, it will run playvideo with the -s flag as 1, the -e flag as 2, and the main argument as game of thrones, i.e. playvideo -s 1 -e 2 game of thrones\n"
    "Because of these options, it is important that the arguments range from most strict to least strict."
    "This means that ~ arguments should probably be at the end.\n"
    "arguments with multiple variables like the play $1 season $2 episode $3 example should be before ones like play... because it will pick the first match\n"
    "You can also put comments if the line starts with # and options if the line starts with a !\nDefault options are shown as follows:\n"
    "!keyword==pi,!verify==1,!continuous==1,!quiet==0,!ignore==0,!thresh=0.7,!response=Yes sir?,!duration==3,!com_dur==2,!filler==1,!hardware==plughw:1,0\n"
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
            string speak;
            if(filler) 
                speak = "tts \"FILL ";
            else 
                speak ="tts-nofill \"";
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

void VoiceCommand::Setup() {
    //Function in order to detect your default options.
    //It will then set these automatically in the config file.
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
        system("tts \"FILL This program was created by Steven Hickson\"");
        printf("Did you hear anything? (y/n)\n");
        scanf("%s",buffer);
        if(buffer[0] == 'n') {
            printf("Something went wrong and you should run the install script and install the dependecies as well\n");
            printf("You might also want to change some alsa options using alsamixer and alsa force-reload\n");
            exit(-1);
        }
        printf("\nIf you heard the word FILL (or FILLER or FILLER FILL) at the beginning of the sentence, the filler flag should be set to 0\n");
        printf("Do you want me to permanently set the filler flag to 0? (y/n)\n");
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            filler = false;
            write += "!filler==0\n";
        }
        printf("\nThe default response of the system after it finds the keyword is \"Yes Sir?\"\n");
        printf("Do you want to change the response? (y/n)\n");
        bool change = false;
        scanf("%s",buffer);
        if(buffer[0] == 'y') {
            change = true;
            while(change) {
                printf("Type the phrase you want as the response:\n");
                scanf("%s",buffer);
                string cmd = "";
                if(filler)
                    cmd += "tts \"FILL ";
                else
                    cmd += "tts-nofill \"";
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
        cmd = popen("arecord -l | awk '/device [0-9]/ {print $2}'","r");
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
        bool change = false;
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
                if(strcmp(message,keyword.c_str()) == 0)
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
