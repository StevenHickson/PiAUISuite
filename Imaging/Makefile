
test: test.cpp
	g++ -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard test.cpp -I/ffmpeg_compiled/usr/local/include -I/usr/include -L/usr/lib/ -L/ffmpeg_compiled/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_objdetect -o test

tracking: tracking.cpp
	g++ -O3 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard tracking.cpp -I/ffmpeg_compiled/usr/local/include -I/usr/include -L/usr/lib/ -L/ffmpeg_compiled/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc -o tracking

clean:
		rm *.o test tracking 2>/dev/null
		
