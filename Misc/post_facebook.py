#!/usr/bin/env python
import smtplib
import string
import sys
import getopt
from email.mime.text import MIMEText

USERNAME = str(sys.argv[1]);
PASSWORD = str(sys.argv[2]);
MAILTO  = "trigger@ifttt.com"

s = ' '.join(sys.argv[3:])
msg = MIMEText(s)
msg['Subject'] = '#facebook'
msg['From'] = USERNAME
msg['To'] = MAILTO

server = smtplib.SMTP('smtp.gmail.com:587')
server.ehlo_or_helo_if_needed()
server.starttls()
server.ehlo_or_helo_if_needed()
server.login(USERNAME,PASSWORD)
server.sendmail(USERNAME, MAILTO, msg.as_string())
server.quit()
