#!/bin/bash

hardware="plughw:1,0"
duration="3"
lang="en"
hw_bool=0
dur_bool=0
lang_bool=0
for var in "$@"
do
    if [ "$var" == "-D" ] ; then
        hw_bool=1
    elif [ "$var" == "-d" ] ; then
        dur_bool=1
    elif [ "$var" == "-l" ] ; then
        lang_bool=1
    elif [ $hw_bool == 1 ] ; then
        hw_bool=0
        hardware="$var"
    elif [ $dur_bool == 1 ] ; then
        dur_bool=0
        duration="$var"
    elif [ $lang_bool == 1 ] ; then
        lang_bool=0
        lang="$var"
    else
        echo "Invalid option, valid options are -D for hardware and -d for duration"
    fi
done

#this works really inconsistently and I don't know why. I would love to implement it
#sox -r 16000 -t alsa $hardware /dev/shm/out.flac silence 1 0.3 1% 1 0.5 1%
#wget -q -U "rate=16000" -O - --post-file /dev/shm/out.flac --header="Content-Type: audio/x-flac; rate=16000" "http://www.google.com/speech-api/v1/recognize?lang=en&client=Mozilla/5.0" | sed -e 's/[{}]/''/g'| awk -v k="text" '{n=split($0,a,","); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'
#arecord -D $hardware -f cd -t wav -d $duration -r 16000 | flac - -f --best --sample-rate 16000 -o /dev/shm/out.flac 1>/dev/shm/voice.log 2>/dev/shm/voice.log; wget -O - -o /dev/null --post-file /dev/shm/out.flac --header="Content-Type: audio/x-flac; rate=16000" http://www.google.com/speech-api/v1/recognize?lang="$lang" | sed -e 's/[{}]/''/g'| awk -v k="text" '{n=split($0,a,","); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'
arecord -D $hardware -f cd -t wav -d $duration -r 16000 | flac - -f --best --sample-rate 16000 -o /dev/shm/out.flac 1>/dev/shm/voice.log 2>/dev/shm/voice.log; curl -X POST --data-binary @/dev/shm/out.flac --user-agent 'Mozilla/5.0' --header 'Content-Type: audio/x-flac; rate=16000;' "https://www.google.com/speech-api/v2/recognize?output=json&lang=$lang&key=AIzaSyBOti4mM-6x9WDnZIjIeyEU21OpBXqWBgw&client=Mozilla/5.0" | sed -e 's/[{}]/''/g' | awk -F":" '{print $4}' | awk -F"," '{print $1}' | tr -d '\n'

rm /dev/shm/out.flac
