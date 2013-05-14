#GITREV  = -D'GITREV="$(shell git log -1 --pretty=format:"%h by %an on %ai")"'
BLDDEF  = -D'BUILDTS="$(shell date +"%y%m%d %H:%M:%S %z")"'

download: download.cpp downconn.o
	g++ $(BLDDEF) -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -lcurl -lboost_regex -o download downconn.o download.cpp

downconn: downconn.cpp downconn.h
	g++ $(BLDDEF) -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -c -lcurl -lboost_regex downconn.cpp

clean:
	rm *.o download 2>/dev/null

install: download
	install download /usr/bin/download
