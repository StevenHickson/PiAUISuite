#!/bin/bash 
user=`cat $HOME/.misc | awk -F'username==' '{print $2}' | tr -d '\n'`
pass=`cat $HOME/.misc | awk -F'password==' '{print $2}' | tr -d '\n'`

okay=0
tts "FILL What would you like to post"
while [ $okay == 0 ]
do
    message=`speech-recog.sh -d 10`
    tts "FILL I got $message. Is this correct?"
    yes=`speech-recog.sh`
    if [[ "$yes" == *"yes"* ]] ; then
        python /home/pi/AUI/Misc/post_facebook.py "$user" "$pass" $message
        okay=1
        exit
    else
        tts "FILL Okay try again"
    fi
done
