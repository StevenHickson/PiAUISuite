#!/bin/bash

#Script to make install of AUI Suite very easy
function dependencies() {
    ARCH=$1
    #Ask to install dependencies
    install_method=`pgrep apt-get`
    if [ -z $install_method ] ; then
    echo "Install dependencies? y/n"
    echo "These are necessary for any of the options, so you should probably press y unless you absolutely know you have them already"
    read option
    if [ $option == "y" ] || [ $option == "Y" ] ; then
        if [ "$ARCH" == "armv6l" ] ; then
            sudo apt-get install locate curl libboost-dev libboost-regex-dev xterm xfonts-base xfonts-utils youtube-dl axel mpg123 libcurl4-gnutls-dev flac sox
        else
            sudo apt-get install locate curl libboost-dev libboost-regex-dev xterm xfonts-base xfonts-utils youtube-dl axel mpg123 libcurl4-gnutls-dev flac sox
        fi
    fi
    fi
}

function playvideo_install() {
    #Install playvideo script
    echo "Install playvideo? y/n"
    echo "This script indexes your movies and tv shows in order to quickly find, randomize, and/or play them. I find it extremely useful"
    read option
    if [ $option == "y" ] || [ $option == "Y" ] ; then
    echo "Installing playvideo"
    echo "Enter movie/video root folder location, ex: /media/External/movies"
    read media_path
    `../PlayVideoScripts/playvideo -set "$media_path"`
    sudo cp ../PlayVideoScripts/playvideo /usr/bin/playvideo
    sudo cp ../PlayVideoScripts/playvideo.8.gz /usr/share/man/man8/
    echo "Done installing playvideo!"
    else
        echo "Skipping playvideo install"
    fi
}

function download() {
    DIR=$1
    USER_HOME="$2"
    #Install downloader script
    echo "Install downloader? y/n"
    echo "This script automates the download of torrents. Ex. 'download wheezy' finds and downloads the newest debian wheezy image"
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
    #print commands to $USER_HOME/.down.info
    if [ -e "$USER_HOME/.down.info" ] ; then
        sudo rm -f "$USER_HOME/.down.info"
    fi
    echo "$host"   | sudo tee -a "$USER_HOME/.down.info" >/dev/null
    echo "$port"   | sudo tee -a "$USER_HOME/.down.info" >/dev/null
    echo "$user"   | sudo tee -a "$USER_HOME/.down.info" >/dev/null
    echo "$passwd" | sudo tee -a "$USER_HOME/.down.info" >/dev/null
    tmp="../DownloadController/"
    tmp+="$DIR"
    tmp+="download"
    sudo cp "$tmp" /usr/bin/download
    echo "Done installing download!"
    else
        echo "Skipping downloader install"
    fi
}

function gvapi() {
    DIR=$1
    USER_HOME="$2"
    #Install gvapi
    echo "Install gvapi (googlevoice api)? y/n"
    echo "This script installs the google voice api. It is really useful for home automation/robotics enthusiasts."
    read option
    if [ $option == "y" ] || [ $option == "Y" ] ; then
        echo "Installing Text Command Script"
        echo "Enter google voice username: "
        read user
        echo "Enter google voice password: "
        read passwd
        #print commands to $USER_HOME/.gtext
        if [ -e "$USER_HOME/.gv" ] ; then
            sudo rm -f "$USER_HOME/.gv"
        fi
        echo "$user"   | sudo tee -a "$USER_HOME/.gv" >/dev/null
        echo "$passwd"   | sudo tee -a "$USER_HOME/.gv" >/dev/null
        tmp="../TextCommand/"
        tmp+="$DIR"
        tmp+="gvapi"
        sudo cp "$tmp" /usr/bin/gvapi
        sudo cp ../TextCommand/gvapi.8.gz /usr/share/man/man8/
        echo "Done installing gvapi!"
    else
        echo "Skipping gvapi install"
    fi
}

function gtextcommand {
    DIR=$1
    USER_HOME="$2"
    #Install gtextcommand script
    echo "Install gtextcommand (google voice text command system)? y/n"
    echo "This installs gtextcommand. This uses google voice to check for system commands from your number with a passcode."
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
    #print commands to $USER_HOME/.gtext
    if [ -e "$USER_HOME/.gtext" ] ; then
        sudo rm -f "$USER_HOME/.gtext"
    fi
    echo "$user"   | sudo tee -a "$USER_HOME/.gtext" >/dev/null
    echo "$passwd"   | sudo tee -a "$USER_HOME/.gtext" >/dev/null
    echo "$key"   | sudo tee -a "$USER_HOME/.gtext" >/dev/null
    echo "$number" | sudo tee -a "$USER_HOME/.gtext" >/dev/null
    tmp="../TextCommand/"
    tmp+="$DIR"
    tmp+="gtextcommand"
    sudo cp "$tmp" /usr/bin/gtextcommand
    #Add to cron.d
    echo "Done installing, setting up cron script ..."
    if [ -e "/etc/cron.d/gtextcommand" ] ; then
        sudo rm -f "/etc/cron.d/gtextcommand"
    fi
    cronjob="#!/bin/sh
    #
    # cron script to check google voice.
    #
    # Written by Steven Hickson <help@stevenhickson.com> for the gtextcommand script.
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
}

function youtube() {
    DIR=$1
    USER_HOME="$2"
    echo "Install youtube scripts? y/n"
    echo "This installs youtube, youtube-safe, youtube-dl, and other scripts that allow you to download, stream, and browse videos from many sites"
    read option
    if [ $option == "y" ] || [ $option == "Y" ] ; then
        tmp="../Youtube/"
        tmp+="$DIR"
        tmp+="youtube-search"
        sudo cp ../Youtube/youtube /usr/bin/
        sudo cp ../Youtube/youtube-safe /usr/bin/
        sudo cp ../Youtube/youtube-dlfast /usr/bin/
        sudo cp "$tmp" /usr/bin/
        sudo cp ../Youtube/update-youtubedl /etc/cron.daily/
        sudo cp ../Youtube/yt.desktop /usr/share/applications/
        sudo cp ../Youtube/ytb.desktop /usr/share/applications/
        mkdir -p "$USER_HOME/.local/share/midori/scripts"
        cp ../Youtube/yt.js "$USER_HOME/.local/share/midori/scripts/" 
        cp ../Youtube/ytb.js "$USER_HOME/.local/share/midori/scripts/" 
        #This however, I'm fairly certain I need
        sudo update-desktop-database
        sudo youtube-dl -U
    else
        echo "Skipping youtube install"
    fi
}

function voicecommand_install() {
    DIR=$1
    USER_HOME="$2"
    echo "Install voicecommand? y/n"
    echo "This is probably the coolest script here and ties many of these together. It is an easily customizable voice control system. It uses speech recognition and text to speech to listen to you, respond to you, and run commands based on what you say."
    read option
    if [ $option == "y" ] || [ $option == "Y" ] ; then
        tmp="../VoiceCommand/"
        tmp+="$DIR"
        tmp+="voicecommand"
        sudo cp "$tmp" /usr/bin/
        sudo cp ../VoiceCommand/google /usr/bin/
        sudo cp ../VoiceCommand/tts /usr/bin/
        sudo cp ../VoiceCommand/speech-recog.sh /usr/bin/
        sudo cp ../VoiceCommand/voicecommand.8.gz /usr/share/man/man8/
        if [[ ! -f "$USER_HOME/.commands.conf" ]] ; then
	    echo "No commands found, using default"
            cp ../VoiceCommand/commands.conf "$USER_HOME/.commands.conf"
	else
	    echo "I found a command file"
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
}

echo "Installing AUI Suite by Steven Hickson"
echo "If you have issues, visit stevenhickson.blogspot.com or email help@stevenhickson.com"

#Get architecture
ARCH=`uname -m`
if [ "$ARCH" == "armv6l" ] ; then
    DIR=""
elif [ "$ARCH" == "x86" ] ; then
    DIR="x86/"
elif [ "$ARCH" == "x86_64" ] ; then
    DIR="x64/"
fi

if [ "$(id -u)" != "0" ]; then
    USER_HOME="$HOME"
else
    USER_HOME="/home/${SUDO_USER}"
fi

ARG="$1"
if [ "$ARG" == "dependencies" ] ; then 
    dependencies "$ARCH"
elif [ "$ARG" == "playvideo" ] ; then 
    playvideo_install
elif [ "$ARG" == "download" ] ; then 
    download "$DIR" "$USER_HOME"
elif [ "$ARG" == "gtextcommand" ] ; then
    gtextcommand "$DIR" "$USER_HOME"
elif [ "$ARG" == "gvapi" ] ; then
    gvapi "$DIR" "$USER_HOME"
elif [ "$ARG" == "youtube" ] ; then
    youtube "$DIR" "$USER_HOME"
elif [ "$ARG" == "voicecommand" ] ; then
    voicecommand_install "$DIR" "$USER_HOME"
else
    dependencies "$ARCH"
    playvideo_install
    download "$DIR" "$USER_HOME"
    gtextcommand "$DIR" "$USER_HOME"
    gvapi "$DIR" "$USER_HOME"
    youtube "$DIR" "$USER_HOME"
    voicecommand_install "$DIR" "$USER_HOME"
fi
