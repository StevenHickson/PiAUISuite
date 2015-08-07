# Alternative User Interface

Includes voicecommand, download, playvideo, and textcommand scripts

This requires:

* boost
* curl
* xterm
* espeak
* some other things

To install the dependencies, run:
```bash
sudo apt-get install -y libboost-dev libboost-regex-dev youtube-dl axel curl xterm libcurl4-gnutls-dev mpg123 flac sox
```

To install PiAUISuite:
```bash
git clone https://github.com/StevenHickson/PiAUISuite.git
cd PiAUISuite/Install
./InstallAUISuite.sh
```

It will:
* ask if you want to install the dependencies
* to install each script

## Different Parts

Name | Purpose | Blogpost
-----|---------|---------
playvideo | finds and plays videos | [Here](http://stevenhickson.blogspot.com/2013/03/playing-videos-intelligently-with.html)
downloader | find and downloads the best torrent | [Here](http://stevenhickson.blogspot.com/2013/03/automatically-downloading-torrents-with.html)
gvapi | checks, sends, and deletes SMS messages | [Here](http://stevenhickson.blogspot.com/2013/05/using-google-voice-c-api.html)
gtextcommand | checks for sms messages every minute and runs commands from them | [Here](http://stevenhickson.blogspot.com/2013/03/controlling-raspberry-pi-via-text.html)
youtube | streams youtube | [In browser](http://stevenhickson.blogspot.com/2013/06/playing-youtube-videos-in-browser-on.html) and [on Pi](http://stevenhickson.blogspot.com/2013/04/using-youtube-on-raspberry-pi-without.html)
youtube-safe | streams other video files | [Hulu and Vimeo](http://stevenhickson.blogspot.com/2013/06/getting-huluvimeo-to-work-on-raspberry.html) and [others](http://stevenhickson.blogspot.com/2013/06/streaming-other-hd-video-sites-on.html)
voicecommand | run voice commands | [Here](http://stevenhickson.blogspot.com/2013/05/voice-command-v20-for-raspberry-pi.html) and [here](http://stevenhickson.blogspot.com/2013/04/voice-control-on-raspberry-pi.html)

Copyright

[GPLv3](https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3))

Steven Hickson
