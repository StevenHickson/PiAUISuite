#!/bin/bash

#Script to make install of AUI Suite very easy

if [ "$(id -u)" != "0" ]; then
    USER_HOME="$HOME"
else
    USER_HOME="/home/${SUDO_USER}"
fi

#Ask to install dependencies

#Uninstall playvideo script
echo "Uninstalling playvideo"
sudo rm -f /usr/bin/playvideo
sudo rm -f "$USER_HOME/.media.db"
sudo rm -f /usr/share/man/man8/playvideo.8.gz
sudo rm -f /etc/cron.daily/updatelocaldb

echo "Uninstalling downloader script"
sudo rm -f "$USER_HOME/.down.info"
sudo rm /usr/bin/download

echo "Uninstalling gvapi script"
sudo rm -f "$USER_HOME/.gv"
sudo rm -f /usr/bin/gvapi
sudo rm -f /usr/share/man/man8/gvapi.8.gz

echo "Uninstalling gtextcommand script"
sudo rm -f "$USER_HOME/.gtext"
sudo rm -f /usr/bin/gtextcommand
sudo rm -f /etc/cron.d/gtextcommand

echo "Uninstalling youtube scripts"
sudo rm -f /usr/bin/youtube
sudo rm -f /usr/bin/youtube-safe
sudo rm -f /usr/bin/youtube-dlfast
sudo rm -f /usr/bin/youtube-search
sudo rm -f /usr/share/applications/yt.desktop
sudo rm -f /usr/share/applications/ytb.desktop
sudo rm -f "$USER_HOME/.local/share/midori/scripts/yt.js"
sudo rm -f "$USER_HOME/.local/share/midori/scripts/ytb.js"

echo "Uninstalling voicecommand scripts"
sudo rm -f /usr/bin/voicecommand
sudo rm -f /usr/bin/google
sudo rm -f /usr/bin/tts
sudo rm -f /usr/bin/tts-nofill
sudo rm -f /usr/bin/speech-recog.sh
sudo rm -f /usr/share/man/man8/voicecommand.8.gz

