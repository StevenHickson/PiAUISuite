#GITREV  = -D'GITREV="$(shell git log -1 --pretty=format:"%h by %an on %ai")"'
BLDDEF  = -D'BUILDTS="$(shell date +"%y%m%d %H:%M:%S %z")"'

gvapi: voicecommand.cpp voicecommand.h
	g++ $(BLDDEF) -g -Wall -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -lcurl -lboost_regex -o voicecommand voicecommand.cpp

clean:
	rm *.o voicecommand 2>/dev/null

install: google speech-recog.sh voicecommand
	install google /usr/bin/google
	install speech-recog.sh /usr/bin/speech-recog.sh
	install voicecommand /usr/bin/voicecommand
