#!/bin/bash

filler="FILLER FILL"
texts=`gvapi -c`
if [ "$texts" != "" ] ; then
    echo "Found a message"
    name=`echo $texts | awk -F':' '{print $1}'`
    message=`echo $texts | awk -F':' '{print $2}'`
    tts "$filler message from $name"
    sleep 0.5
    tts "$filler $message"
else
    echo "No messages"
    tts "$filler no messages"
fi
