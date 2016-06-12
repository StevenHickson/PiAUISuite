import pywapi
import string
import sys

#this requires pywapi which can be installed from https://code.google.com/p/python-weather-api/
#If in the US 00000 should be your area code
#If in Britain, try using the BBC weather scripts found here: https://github.com/duncanj/voice_scripts

# pass a second parameter, 'imperial' if you are feeling like ferinheight instead of celsius
result = pywapi.get_weather_from_weather_com(str(sys.argv[1]))

print "It is " + string.lower(result['current_conditions']['text']) + " and " + result['current_conditions']['temperature'] + " degrees."
print "Humidity " + result['current_conditions']['humidity'] + " percent."

today = result['forecasts'][0]
highToday = today['high']
if highToday > result['current_conditions']['temperature']:
    print "The expected high is " + highToday + "."

print "Overnight, " + today['night']['text'] + " with a low of " + today['low'] + " "
if today['night']['chance_precip'] != "0":
    print " and a " + today['night']['chance_precip'] + " percent chance of rain."

tomorrow = result['forecasts'][1]
print "Tomorrow, "+ tomorrow['day']['text'] + " with a high of " + tomorrow['high'] + " "

if tomorrow['day']['chance_precip'] != "0":
    print " and a " + tomorrow['day']['chance_precip'] + " percent chance of rain."
