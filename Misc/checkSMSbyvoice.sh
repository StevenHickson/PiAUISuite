#!/bin/bash

texts=`gvapi -c`
if [ "$texts" != "" ] ; then
    echo "Found a message"
    name=`echo $texts | awk -F':' '{print $1}'`
    message=`echo $texts | awk -F':' '{print $2}'`
    tts "FILL message from $name"
    sleep 0.5
    tts "FILL $message"
else
    echo "No messages"
    tts "FILL no messages"
fi
