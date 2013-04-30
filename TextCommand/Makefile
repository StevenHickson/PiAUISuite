#GITREV  = -D'GITREV="$(shell git log -1 --pretty=format:"%h by %an on %ai")"'
BLDDEF  = -D'BUILDTS="$(shell date +"%y%m%d %H:%M:%S %z")"'

gvapi: gtextcommand.cpp gvoice.cpp gvoice.h gvoice
	g++ $(BLDDEF) -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -lcurl -lboost_regex -o gtextcommand gvoice.o gtextcommand.cpp

gvoice: gvoice.cpp gvoice.h
	g++ $(BLDDEF) -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -c -lcurl -lboost_regex gvoice.cpp

clean:
	rm *.o gtextcommand 2>/dev/null

install: gtextcommand
	install gtextcommand /usr/bin/gtextcommand
