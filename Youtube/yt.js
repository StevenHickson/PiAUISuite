// YouTube  Launcher
// Steven Hickson
// 2013-06-07
// Copyright (c) 2013
// Released under the GPL license
// http://www.gnu.org/copyleft/gpl.html
// Modified version of BlockFlash2 by varanasi,Andrew Pennebaker,Jos van den Oever
// Modified to work with omxplayer and not use flash at all
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

var ytbody = document.getElementsByTagName('body')[0];
var ytdiv = document.createElement('div');
var ythref = location.href;
ythref=ythref.replace('https://','yt://');
ythref=ythref.replace('http://','yt://');
ytdiv.innerHTML = '<a href="'+ythref+'">OMXPlayer |&gt;</a>';
ytdiv.style.backgroundColor = '#f3f3f3';
ytdiv.style.border='1px solid black';
ytdiv.style.position = 'fixed';
ytdiv.style.top = '10px';
ytdiv.style.left = '10px';
ytbody.appendChild(ytdiv);


/*xpath("//embed").forEach(function(embed) {            // put all embed objects in array and check each
    if (embed.parentNode.nodeName != "OBJECT" && embed.parentNode.style.display != "none"){       // handle embeds within objects as objects
	if(checkforflash(embed)){add_play_flash_div(embed)};
   };
});

xpath("//object").forEach(function(object) {     
    if(checkforflash(object)){add_play_flash_div(object)};
});
*/
/*
function checkforflash(potl_item){                    // checks the element passed to it for Flash content
    if (potl_item.hasAttribute("flashvars") ){
	return true
    };
    if (potl_item.hasAttribute("type") && potl_item.getAttribute("type").match(/flash|shockwave/)){
	return true
    };
    if (potl_item.hasAttribute("src") && potl_item.getAttribute("src").match(/.swf|shockwave|flash|eyewonder/)){
	return true
    };
    if (potl_item.innerHTML.match(/.swf|shockwave|flash|eyewonder/)) {
	return true
    };
    if (potl_item.hasAttribute("class") && potl_item.getAtrribute("class").match(/ytp/)){
    return true
    };
    return false;
};

function add_play_flash_div(flash){            // places the button-like div before the flash node
    var ytbody = document.getElementsByTagName('body')[0];
    var ytdiv = document.createElement('div');
    var ythref = location.href;
    ythref=ythref.replace('https://','yt://');
    ythref=ythref.replace('http://','yt://');
    ytdiv.innerHTML = '<a href="'+ythref+'">OMX |&gt;</a>';
    ytdiv.style.backgroundColor = '#f3f3f3';
    ytdiv.style.border='1px solid black';
    ytdiv.style.position = 'fixed';
    ytdiv.style.top = '10px';
    ytdiv.style.left = '10px';
    ytbody.appendChild(ytdiv);
    flash.remove();
    return true;
}

all_objects = document.getElementsByTagName('object');
all_embeds = document.getElementsByTagName('embed');
all_divs = document.getElementsByTagName('div');

for (var i = 0; i < all_ojects.length; i++) {
    if(checkforflash(all_objects[i]) {
        add_play_flash_div(all_objects[i]);
    }
}

for (var i = 0; i < all_embeds.length; i++) {
    if(checkforflash(all_embeds[i]) {
        add_play_flash_div(all_embeds[i]);
    }
}

for (var i = 0; i < all_divs.length; i++) {
    if(checkforflash(all_divs[i]) {
        add_play_flash_div(all_divs[i]);
    }
}
*/
