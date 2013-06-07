#!/bin/bash

#Message from: name says: message
user=`cat $HOME/.misc | awk -F'username==' '{print $2}' | tr -d '\n'`
pass=`cat $HOME/.misc | awk -F'password==' '{print $2}' | tr -d '\n'`

texts=`python /home/pi/AUI/Misc/check_gmail.py "$user" "$pass"`
if [ "$texts" != "" ] ; then
    echo "Found a message"
    printf "%s\n" "$texts" | 
    while IFS= read -r line
    do
        name=${line#*from:}
        name=${name%says:*}
        message=${line#*says:}
        echo "message from $name"
        tts "FILL message from $name"
        sleep 0.5
        echo "    $message"
        tts "FILL $message"
    done 
else
    echo "No messages"
    tts "FILL no messages"
fi
