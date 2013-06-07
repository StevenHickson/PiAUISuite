#!/usr/bin/env python
import feedparser
import string
import sys

USERNAME = str(sys.argv[1]);
PASSWORD = str(sys.argv[2]);

response = feedparser.parse("https://" + USERNAME + ":" + PASSWORD + "@mail.google.com/gmail/feed/atom")
unread_count = int(response["feed"]["fullcount"])

if unread_count > 20:
    unread_count = 20

for i in range(0,unread_count):
    name = response['items'][i].author.split("(");
    print "Message from: " + name[0] + "says: " + response['items'][i].title
