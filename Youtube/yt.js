// YouTube  Launcher
// Steven Hickson
// 2013-06-07
// Copyright (c) 2013
// Released under the GPL license
// http://www.gnu.org/copyleft/gpl.html
//
// --------------------------------------------------------------------
//
// ==UserScript==
// @name			YouTube Launcher
// @namespace		None
// @description		Plays youtube videos with omxplayer
// @include			http*://youtube.com/*
// @include			http*://*.youtube.com/*
// @include			http*://youtube-nocookie.com/*
// @include			http*://*.youtube-nocookie.com/*
//
// ==/UserScript==
var url1,url2;
url1 = ['www.youtube.com','youtube.com', 'www.video.google.com', 'video.google.com', 'watch?', '/watch?'];
var a, links;
var tmp="a";
var p,q;
links = document.getElementsByTagName('a');
for (var i = 0; i < links.length; i++) {
    a = links[i];
    for(var j=0;j<url1.length; j++) {
        tmp = a.href+"" ;
        if(tmp.indexOf(url1[j]) != -1) {
            a.href = tmp.replace("http","yt");
        }
    }
}
