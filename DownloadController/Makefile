#GITREV  = -D'GITREV="$(shell git log -1 --pretty=format:"%h by %an on %ai")"'
BLDDEF  = -D'BUILDTS="$(shell date +"%y%m%d %H:%M:%S %z")"'

#Set Architecture
ARCH := arm
DEVEL := rel
LIBS := -lcurl -lboost_regex

#Compilers
ifeq ($(ARCH),arm)
CC := g++ -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard
else
ifeq ($(ARCH),x86)
CC := g++ -m32
OUTPUT := x86/
else
CC := g++
OUTPUT := x64/
endif
endif

#devel flags
ifeq ($(BUILD),devel)
FLAGS := -g -Wall
else
FLAGS := -O3
endif

download: download.cpp downconn.cpp downconn.h
	$(CC) $(BLDDEF) $(FLAGS) $(LIBS) -o $(addprefix $(OUTPUT), download) downconn.cpp download.cpp

clean:
	rm *.o download 2>/dev/null

install: download
	install download /usr/bin/download
