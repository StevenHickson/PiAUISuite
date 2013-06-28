#!/bin/bash

#Script to make install of AUI Suite very easy

function playvideo() {
    USER_HOME="$1"
    #Uninstall playvideo script
    echo "Uninstalling playvideo"
    sudo rm -f /usr/bin/playvideo
    sudo rm -f "$USER_HOME/.media.db"
    sudo rm -f /usr/share/man/man8/playvideo.8.gz
    sudo rm -f /etc/cron.daily/updatelocaldb
}

function download() {
    USER_HOME="$1"
    echo "Uninstalling downloader script"
    sudo rm -f "$USER_HOME/.down.info"
    sudo rm /usr/bin/download
}

function gvapi() {
    USER_HOME="$1"
    echo "Uninstalling gvapi script"
    sudo rm -f "$USER_HOME/.gv"
    sudo rm -f /usr/bin/gvapi
    sudo rm -f /usr/share/man/man8/gvapi.8.gz
}

function gtextcommand {
    USER_HOME="$1"
    echo "Uninstalling gtextcommand script"
    sudo rm -f "$USER_HOME/.gtext"
    sudo rm -f /usr/bin/gtextcommand
    sudo rm -f /etc/cron.d/gtextcommand
}

function youtube() {
    USER_HOME="$1"
    echo "Uninstalling youtube scripts"
    sudo rm -f /usr/bin/youtube
    sudo rm -f /usr/bin/youtube-safe
    sudo rm -f /usr/bin/youtube-dlfast
    sudo rm -f /usr/bin/youtube-search
    sudo rm -f /usr/share/applications/yt.desktop
    sudo rm -f /usr/share/applications/ytb.desktop
    sudo rm -f "$USER_HOME/.local/share/midori/scripts/yt.js"
    sudo rm -f "$USER_HOME/.local/share/midori/scripts/ytb.js"
}

function voicecommand() {
    echo "Uninstalling voicecommand scripts"
    sudo rm -f /usr/bin/voicecommand
    sudo rm -f /usr/bin/google
    sudo rm -f /usr/bin/tts
    sudo rm -f /usr/bin/speech-recog.sh
    sudo rm -f /usr/share/man/man8/voicecommand.8.gz
}

if [ "$(id -u)" != "0" ]; then
    USER_HOME="$HOME"
else
    USER_HOME="/home/${SUDO_USER}"
fi

ARG="$1"
if [ "$ARG" == "playvideo" ] ; then 
    playvideo "$USER_HOME"
elif [ "$ARG" == "download" ] ; then 
    download "$USER_HOME"
elif [ "$ARG" == "gtextcommand" ] ; then
    gtextcommand "$USER_HOME"
elif [ "$ARG" == "gvapi" ] ; then
    gvapi "$USER_HOME"
elif [ "$ARG" == "youtube" ] ; then
    youtube "$USER_HOME"
elif [ "$ARG" == "voicecommand" ] ; then
    voicecommand "$USER_HOME"
else
    playvideo "$USER_HOME"
    download "$USER_HOME"
    gtextcommand "$USER_HOME"
    gvapi "$USER_HOME"
    youtube "$USER_HOME"
    voicecommand "$USER_HOME"
fi
