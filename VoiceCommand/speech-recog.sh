#!/bin/bash

arecord -D plughw:1,0 -f cd -t wav -d 3 -r 16000 | flac - -f --best --sample-rate 16000 -o /dev/shm/out.flac 1>/dev/null 2>/dev/null; wget -O - -o /dev/null --post-file /dev/shm/out.flac --header="Content-Type: audio/x-flac; rate=16000" http://www.google.com/speech-api/v1/recognize?lang=en | sed -e 's/[{}]/''/g'| awk -v k="text" '{n=split($0,a,","); for (i=1; i<=n; i++) print a[i]; exit }' | awk -F: 'NR==3 { print $3; exit }'
rm /dev/shm/out.flac
