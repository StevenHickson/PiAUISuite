#!/bin/bash

result=`python /home/pi/AUI/Misc/getweather.py`

echo "$result"
tts "$result"

