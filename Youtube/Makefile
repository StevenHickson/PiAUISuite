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

youtube-search: youtube-search.h youtube-search.cpp
	$(CC) $(BLDDEF) $(FLAGS) $(LIBS) -o $(addprefix $(OUTPUT), youtube-search) youtube-search.cpp

clean:
	rm *.o youtube-search 2>/dev/null

install: youtube youtube-dlfast youtube-search
	install youtube /usr/bin/youtube
	install youtube-dlfast /usr/bin/youtube-dlfast
	install youtube-search /usr/bin/youtube-search
