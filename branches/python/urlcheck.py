"""
  URL Checker program
  
  Simple program to check URLs and their status codes. The URLs must be in a file,
  one URL per line.
  
  Usage: python urlcheck.py <url> <file>
  Parameters:
    url: The URL to scan (e.g. www.google.com)
    file: The file with the URLs
    
  Author: Martin Albrecht <martin.albrecht@javacoffee.de>
  Version: 0.1

  Copyright (C) 2010 Martin Albrecht <martin.albrecht@javacoffee.de>

  This library is free software; you can redistribute it and/or modify it under 
  the terms of the GNU Lesser General Public License as published by the Free 
  Software Foundation; either version 2.1 of the License, or (at your option) 
  any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT 
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
  FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
  details.
  You should have received a copy of the GNU Lesser General Public License along 
  with this library; if not, write to the Free Software Foundation, Inc., 
  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
"""
import httplib
import sys
import re

# Check command line arguments
if len(sys.argv) >= 3:
	url_scan = sys.argv[1]
	filename = sys.argv[2]		
else:
	print "usage: python " + sys.argv[0] + " <url to scan> <url file>"
	sys.exit(1)

# Read in URL file
urls = []
fd = open(filename, "r")
for line in fd:
	if line != "\n" and line.rstrip() not in urls:
		urls.append(line.rstrip())
fd.close()
		
# Open output file
out = open('invalid_urls.csv', 'w+')
if not out:
	print "ERROR: Cannot open output file!"
	sys.exit(1)

# Start to scan
user_agent = "Mozilla/5.0 (OS/2; U; Warp 4.5; de; rv:1.8.1.11) Gecko/20071129 PmWFx/2.0.0.11"
headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain","User-Agent:": user_agent}
rounds = len(urls)
print "Read in ", rounds, " URLs, scanning now..."
out_msg = 'STATUS,ORIGINAL LOCATION,NEW LOCATION,NEW LOC. STATUS\n'

for r in range(0,rounds):
	if "/" not in urls[r]:
		url = "/"+urls[r]
	else:
		url = urls[r]
		
	res = re.match("http://([^/]+)(.*)", url)
	if res and res.group(1):
	  url_scan = res.group(1)
	  url = res.group(2)
	else:
	  url_scan = sys.argv[1]
	  
	conn = httplib.HTTPConnection(url_scan)
	conn.request("HEAD", url)	
	if not conn:
		print "ERROR: Cannot set connection to "+url_scan
		sys.exit(1)
		
	res = conn.getresponse()	
	new_loc = '';
	if res.status != 200:
		new_loc = res.getheader('location')
		
		if new_loc:
			conn2 = httplib.HTTPConnection(url_scan)
			conn2.request("HEAD", new_loc)
			if conn2:
				res2 = conn2.getresponse()
				conn2.close()
			else:
				res2.status = ''				
				
			out_msg += str(res.status)+","+urls[r]+","+new_loc+","+str(res2.status)+"\r\n"
		else:
			out_msg += str(res.status)+","+urls[r]+",\r\n"
	conn.close()

out.write(out_msg)
out.close()
print "All done!"
