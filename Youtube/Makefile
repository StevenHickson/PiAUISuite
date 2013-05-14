#GITREV  = -D'GITREV="$(shell git log -1 --pretty=format:"%h by %an on %ai")"'
BLDDEF  = -D'BUILDTS="$(shell date +"%y%m%d %H:%M:%S %z")"'

youtube-search: youtube-search.h youtube-search.cpp
	g++ $(BLDDEF) -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -lcurl -lboost_regex -o youtube-search youtube-search.cpp

clean:
	rm *.o youtube-search 2>/dev/null

install: youtube youtube-dlfast youtube-search
	install youtube /usr/bin/youtube
	install youtube-dlfast /usr/bin/youtube-dlfast
	install youtube-search /usr/bin/youtube-search
