import pywapi
import string
import sys

#this requires pywapi which can be installed from https://code.google.com/p/python-weather-api/
#If in the US 00000 should be your area code
#If in Britian, try using the BBC weather scripts found here: https://github.com/duncanj/voice_scripts

result = pywapi.get_weather_from_weather_com(str(sys.argv[1]))

print "It is " + string.lower(result['current_conditions']['text']) + " and " + result['current_conditions']['temperature'] + "C now."

