#!/bin/bash

#Script to make install of AUI Suite very easy
echo "Installing AUI Suite by Steven Hickson"
echo "If you have issues, visit stevenhickson.blogspot.com or email me@stevenhickson.com"

#Ask to install dependencies
install_method=`ps -ea | grep apt-get`
if [ -z $install_method ] ; then
echo "Install dependencies? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
    sudo apt-get install locate curl libboost1.50-dev libboost-regex1.50-dev xterm xfonts-base xfonts-utils youtube-dl axel mpg123 libcurl4-gnutls-dev flac sox
fi
fi


#Install playvideo script
echo "Install playvideo? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
echo "Installing playvideo"
echo "Enter movie/video root folder location, ex: /media/External/movies"
read media_path
`../PlayVideoScripts/playvideo.sh -set "$media_path"`
sudo cp ../PlayVideoScripts/playvideo /usr/bin/playvideo
sudo cp ../PlayVideoScripts/playvideo.8.gz /usr/share/man/man8/
echo "Done installing playvideo!"
else
    echo "Skipping playvideo install"
fi

#Install downloader script
echo "Install downloader? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
echo "Installing downloader script"
echo "Enter host: ex, localhost (this is probably what it is)"
read host
echo "Enter Port: default is 9091 I believe"
read port
echo "Enter username, press enter if none"
read user
echo "Enter password, press enter if none"
read passwd
#print commands to $HOME/.down.info
if [ -e "$HOME/.down.info" ] ; then
    sudo rm -f "$HOME/.down.info"
fi
echo "$host"   | sudo tee -a "$HOME/.down.info" >/dev/null
echo "$port"   | sudo tee -a "$HOME/.down.info" >/dev/null
echo "$user"   | sudo tee -a "$HOME/.down.info" >/dev/null
echo "$passwd" | sudo tee -a "$HOME/.down.info" >/dev/null
sudo cp ../DownloadController/download /usr/bin/download
echo "Done installing download!"
else
    echo "Skipping downloader install"
fi

#Install gvapi
echo "Install gvapi (googlevoice api)? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
echo "Installing Text Command Script"
echo "Enter google voice username: "
read user
echo "Enter google voice password: "
read passwd
#print commands to $HOME/.gtext
if [ -e "$HOME/.gv" ] ; then
    sudo rm -f "$HOME/.gv"
fi
echo "$user"   | sudo tee -a "$HOME/.gv" >/dev/null
echo "$passwd"   | sudo tee -a "$HOME/.gv" >/dev/null
sudo cp ../TextCommand/gvapi /usr/bin/gvapi
sudo cp ../TextCommand/gvapi.8.gz /usr/share/man/man8/
echo "Done installing gvapi!"
else
    echo "Skipping gvapi install"
fi

#Install gtextcommand script
echo "Install gtextcommand (google voice text command system)? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
echo "Installing Text Command Script"
echo "Enter google voice username: "
read user
echo "Enter google voice password: "
read passwd
echo "Enter command keyword, ex: Cmd (this will proceed every vaild command)"
read key
echo "Enter valid number to accept commands from (make sure to put the country code but not the +) ex: 15553334444"
read number
#print commands to $HOME/.gtext
if [ -e "$HOME/.gtext" ] ; then
    sudo rm -f "$HOME/.gtext"
fi
echo "$user"   | sudo tee -a "$HOME/.gtext" >/dev/null
echo "$passwd"   | sudo tee -a "$HOME/.gtext" >/dev/null
echo "$key"   | sudo tee -a "$HOME/.gtext" >/dev/null
echo "$number" | sudo tee -a "$HOME/.gtext" >/dev/null
sudo cp ../TextCommand/gtextcommand /usr/bin/gtextcommand
#Add to cron.d
echo "Done installing, setting up cron script ..."
if [ -e "/etc/cron.d/gtextcommand" ] ; then
    sudo rm -f "/etc/cron.d/gtextcommand"
fi
cronjob="#!/bin/sh
#
# cron script to check google voice.
#
# Written by Steven Hickson <me@stevenhickson.com> for the gtextcommand script.
#
DISPLAY=:0

* * * * * $USER gtextcommand

"
echo "$cronjob" | sudo tee -a /etc/cron.d/gtextcommand >/dev/null
sudo sh -c 'chmod +x /etc/cron.d/gtextcommand'
echo "Done installing gtextcommand!"
else
    echo "Skipping gtextcommand install"
fi

echo "Install youtube scripts? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
    sudo cp ../Youtube/youtube /usr/bin/
    sudo cp ../Youtube/youtube-dlfast /usr/bin/
    sudo cp ../Youtube/youtube-search /usr/bin/
    sudo cp ../Youtube/yt.desktop /usr/share/applications/
    mkdir -p "$HOME/.local/share/midori/scripts"
    cp ../Youtube/yt.js "$HOME/.local/share/midori/scripts/" 
    #I don't know if you need to do this. It kind of seems like gconftool doesn't work anymore, but just in case
    gconftool-2 -s /desktop/gnome/url-handlers/yt/command '/usr/bin/youtube %s' --type String
    gconftool-2 -s /desktop/gnome/url-handlers/yt/enabled --type Boolean true
    #This however, I'm fairly certain I need
    sudo update-desktop-database
else
    echo "Skipping youtube install"
fi

echo "Install voicecommand? y/n"
read option
if [ $option == "y" ] || [ $option == "Y" ] ; then
    sudo cp ../VoiceCommand/voicecommand /usr/bin/
    sudo cp ../VoiceCommand/google /usr/bin/
    sudo cp ../VoiceCommand/tts /usr/bin/
    sudo cp ../VoiceCommand/tts-nofill /usr/bin/
    sudo cp ../VoiceCommand/speech-recog.sh /usr/bin/
    sudo cp ../VoiceCommand/voicecommand.8.gz /usr/share/man/man8/
    if [ -z "$HOME/.commands.conf" ] ; then
        cp ../VoiceCommand/commands.conf "$HOME/.commands.conf"
    fi
    echo "Would you like voicecommand to try to set itself up? y/n"
    read option
    if [ $option == "y" ] || [ $option == "Y" ] ; then
        voicecommand -s
    else
        echo "You can do this manually at any time by running voicecommand -s"
    fi
else
    echo "Skipping voicecommand install"
fi
    
