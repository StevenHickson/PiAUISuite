#!/bin/bash

hardware="plughw:1,0"
duration="3"
hw_bool=0
dur_bool=0

for var in "$@"
do
    if [ "$var" == "-D" ] ; then
        hw_bool=1
    elif [ "$var" == "-d" ] ; then
        dur_bool=1
    elif [ $hw_bool == 1 ] ; then
        hw_bool=0
        hardware="$var"
    elif [ $dur_bool == 1 ] ; then
        dur_bool=0
        duration="$var"
    else
        echo "Invalid option, valid options are -D for hardware and -d for duration"
    fi
done

arecord -D $hardware -f cd -t wav -d $duration -r 16000 | flac - -f --best --sample-rate 16000 -o /dev/shm/out.flac 1>/dev/null 2>/dev/null; wget -O - -o /dev/null --post-file /dev/shm/out.flac --header="Content-Type: audio/x-flac; rate=16000" http://www.google.com/speech-api/v1/recognize?lang=en | sed -e 's/[{}]/''/g'| awk -v k="text" '{n=split($0,a,","); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'
rm /dev/shm/out.flac
