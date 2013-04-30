
all:
	cd DownloadController; make
	cd Imaging; make
	cd TextCommand; make
	cd VoiceCommand; make
	cd Youtube; make

clean:
	cd DownloadController; make clean
	cd Imaging; make clean
	cd TextCommand; make clean
	cd VoiceCommand; make clean
	cd Youtube; make clean

install:
	cd Install; ./InstallAUISuite.sh
