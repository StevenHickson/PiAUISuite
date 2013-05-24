import  pywapi
import string

result = pywapi.get_weather_from_weather_com('21144')

print "It is " + string.lower(result['current_conditions']['text']) + " and " + result['current_conditions']['temperature'] + "C now."

