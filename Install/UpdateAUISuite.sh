#!/bin/bash

# Make sure only root can run our script
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

#Get architecture
ARCH=`uname -m`
if [ "$ARCH" == "armv6l" ] ; then
    DIR=""
elif [ "$ARCH" == "x86" ] ; then
    DIR="x86/"
elif [ "$ARCH" == "x86_64" ] ; then
    DIR="x64/"
fi
ROOT_DIR="https://raw.github.com/StevenHickson/PiAUISuite/master"

#I should first try to update the Update script
wget https://raw.github.com/StevenHickson/PiAUISuite/master/Install/UpdateAUISuite.sh
difference=`diff UpdateAUISuite.sh UpdateAUISuite.sh.1`

if [ -n "$difference" ] ; then
    echo "I found a new version of the update script"
    unlink UpdateAUISuite.sh
    mv UpdateAUISuite.sh.1 UpdateAUISuite.sh
    chmod +x UpdateAUISuite.sh
    ./UpdateAUISuite.sh
    exit
else
    rm UpdateAUISuite.sh.1
fi

if [ -e "/usr/bin/playvideo" ] ; then
    echo "Updating playvideo ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/PlayVideoScripts/playvideo
    chmod +x /usr/bin/playvideo
    wget -N -P /usr/share/man/man8/ https://raw.github.com/StevenHickson/PiAUISuite/master/PlayVideoScripts/playvideo.8.gz
fi

if [ -e "/usr/bin/download" ] ; then
    echo "Updating download ..."
    tmp="$ROOT_DIR/DownloadController/"
    tmp+="$DIR"
    tmp+="download"
    wget -N -P /usr/bin/ "$tmp"
    chmod +x /usr/bin/download
fi

if [ -e "/usr/bin/gtextcommand" ] ; then
    echo "Updating gtextcommand ..."
    tmp="$ROOT_DIR/TextCommand/"
    tmp+="$DIR"
    tmp+="gtextcommand"
    wget -N -P /usr/bin/ "$tmp"
    chmod +x /usr/bin/gtextcommand
fi

if [ -e "/usr/bin/gvapi" ] ; then
    echo "Updating gvapi ..."
    tmp="$ROOT_DIR/TextCommand/"
    tmp+="$DIR"
    tmp+="gvapi"
    wget -N -P /usr/bin/ "$tmp"
    chmod +x /usr/bin/gvapi
    wget -N -P /usr/share/man/man8/ https://raw.github.com/StevenHickson/PiAUISuite/master/TextCommand/gvapi.8.gz
fi

if [ -e "/usr/bin/youtube" ] ; then
    echo "Updating youtube ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/youtube
    chmod +x /usr/bin/youtube
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/youtube-safe
    chmod +x /usr/bin/youtube-safe
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/youtube-dlfast
    chmod +x /usr/bin/youtube-dlfast
    tmp="$ROOT_DIR/Youtube/"
    tmp+="$DIR"
    tmp+="youtube-search"
    wget -N -P /usr/bin/ "$tmp"
    chmod +x /usr/bin/youtube-search
    wget -N -P /usr/share/applications/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/yt.desktop
    mkdir -p "$HOME/.local/share/midori/scripts"
    wget -N -P "$HOME/.local/share/midori/scripts/" https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/yt.js
    update-desktop-database
    youtube-dl -U
fi

if [ -e "/usr/bin/voicecommand" ] ; then
    echo "Updating voicecommand ..."
    tmp="$ROOT_DIR/VoiceCommand/"
    tmp+="$DIR"
    tmp+="voicecommand"
    wget -N -P /usr/bin/ "$tmp"
    chmod +x /usr/bin/voicecommand
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/VoiceCommand/google
    chmod +x /usr/bin/google
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/VoiceCommand/tts
    chmod +x /usr/bin/tts
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/VoiceCommand/tts-nofill
    chmod +x /usr/bin/tts-nofill
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/VoiceCommand/speech-recog.sh
    chmod +x /usr/bin/speech-recog.sh
    wget -N -P /usr/share/man/man8/ https://raw.github.com/StevenHickson/PiAUISuite/master/VoiceCommand/voicecommand.8.gz
fi
