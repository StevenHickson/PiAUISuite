#!/bin/bash

# Make sure only root can run our script
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

#I should first try to update the Update script
wget https://raw.github.com/StevenHickson/PiAUISuite/master/Install/UpdateAUISuite.sh
difference=`diff UpdateAUISuite.sh UpdateAUISuite.sh.1`

if [ -e $difference ] ; then
    unlink UpdateAUISuite.sh
    mv UpdateAUISuite.sh.1 UpdateAUISuite.sh
    chmod +x UpdateAUISuite.sh
    ./UpdateAUISuite.sh
fi

if [ -e "/usr/bin/playvideo" ] ; then
    echo "Updating playvideo ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/PlayVideoScripts/playvideo
    chmod +x /usr/bin/playvideo
    wget -N -P /usr/share/man/man8/ https://raw.github.com/StevenHickson/PiAUISuite/master/PlayVideoScripts/playvideo.8.gz
fi

if [ -e "/usr/bin/download" ] ; then
    echo "Updating download ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/DownloadController/download
    chmod +x /usr/bin/download
fi

if [ -e "/usr/bin/gtextcommand" ] ; then
    echo "Updating gtextcommand ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/TextCommand/gtextcommand
    chmod +x /usr/bin/gtextcommand
fi

if [ -e "/usr/bin/youtube" ] ; then
    echo "Updating youtube ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/youtube
    chmod +x /usr/bin/youtube
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/youtube-dlfast
    chmod +x /usr/bin/youtube-dlfast
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/Youtube/youtube-search
    chmod +x /usr/bin/youtube-search
fi

if [ -e "/usr/bin/voicecommand" ] ; then
    echo "Updating voicecommand ..."
    wget -N -P /usr/bin/ https://raw.github.com/StevenHickson/PiAUISuite/master/VoiceCommand/voicecommand
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
