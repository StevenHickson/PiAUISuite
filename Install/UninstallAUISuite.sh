#!/bin/bash

#Script to make install of AUI Suite very easy

#Uninstall playvideo script
echo "Uninstalling playvideo"
sudo rm -f /usr/bin/playvideo
sudo rm -f "$HOME/.media.db"
sudo rm -f /usr/share/man/man8/playvideo.8.gz
sudo rm -f /etc/cron.daily/updatelocaldb

echo "Uninstalling downloader script"
sudo rm -f "$HOME/.down.info"
sudo rm /usr/bin/download

echo "Uninstalling gtextcommand script"
sudo rm -f "$HOME/.gtext"
sudo rm -f /usr/bin/gtextcommand
sudo rm -f /etc/cron.d/gtextcommand

echo "Uninstalling youtube scripts"
sudo rm -f /usr/bin/youtube
sudo rm -f /usr/bin/youtube-dlfast
sudo rm -f /usr/bin/youtube-search

echo "Uninstalling voicecommand scripts"
sudo rm -f /usr/bin/voicecommand
sudo rm -f /usr/bin/google
sudo rm -f /usr/bin/tts
sudo rm -f /usr/bin/speech-recog.sh
sudo rm -f /usr/share/man/man8/voicecommand.8.gz

