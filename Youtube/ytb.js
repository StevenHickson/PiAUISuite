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
// @include         *
// @exclude			http*://youtube.com/*
// @exclude			http*://*.youtube.com/*
// @exclude			http*://youtube-nocookie.com/*
// @exclude			http*://*.youtube-nocookie.com/*
//
// ==/UserScript==

var ytbody = document.getElementsByTagName('body')[0];
var ytdiv = document.createElement('div');
var ythref = location.href;
ythref=ythref.replace('https://','ytb://');
ythref=ythref.replace('http://','ytb://');
ytdiv.innerHTML = '<a href="'+ythref+'">OMXPlayer |&gt;</a>';
ytdiv.style.backgroundColor = '#f3f3f3';
ytdiv.style.border='1px solid black';
ytdiv.style.position = 'fixed';
ytdiv.style.top = '10px';
ytdiv.style.left = '10px';
ytbody.appendChild(ytdiv);

/*
xpath("//embed").forEach(function(embed) {            // put all embed objects in array and check each
    if (embed.parentNode.nodeName != "OBJECT" && embed.parentNode.style.display != "none"){       // handle embeds within objects as objects
	if(checkforflash(embed)){add_play_flash_div(embed)};
   };
});

xpath("//object").forEach(function(object) {     
    if(checkforflash(object)){add_play_flash_div(object)};
});

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
    return false;
};

function add_play_flash_div(flash){            // places the button-like div before the flash node
    var placeholder=document.createElement("a");
    savedDisplay = flash.style.display;
    placeholder.setAttribute("class", "ReplaceVideo");
    flash.parentNode.insertBefore(placeholder, flash);  
    flash.style.display='none';               
    flash.on=false;
    flash.remove();
    placeholder.style.cursor='pointer';
    placeholder.style.background='green';  
    placeholder.style.textAlign='center';
    placeholder.style.textTransform='capitalize';
    placeholder.style.color='black';
    placeholder.innerHTML="[Play Video]";
    var tmp=document.location.href;
    tmp=tmp.replace("https","ytb");
    placeholder.href=tmp.replace("http","ytb");
    return true;
}

function xpath (p, context) {
    if (!context) context = document;
    var i, arr = [], xpr = document.evaluate(p, context, null, XPathResult.UNORDERED_NODE_SNAPSHOT_TYPE, null);
    for (i = 0; item = xpr.snapshotItem(i); i++) arr.push(item);
    return arr;
};
*/
