#!/bin/bash

zipcode=`cat $HOME/.misc | awk -F'zipcode==' '{print $2}'`
result=`python /home/pi/AUI/Misc/getweather.py "$zipcode"`

echo "$result"
tts "FILLER FILL $result"

