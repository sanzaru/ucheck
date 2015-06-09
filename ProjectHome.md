# URL check program #
This program checks the HTTP status for given URLs. The URLs should be in a
text file (one URL per line) and they MUST be relative.
To check the URL the program fetches the HTTP header for the given URL and
validates the returned status.

Usage example: ./checker -u www.yourdomain.com -f urls\_to\_check.txt
Or simply: ./checker -h
to see a help screen with all available options.

### UPDATE ###
_There is a new branch available coded in python. This time the scan is more accurate and all types of URLs (relative and absolute can be scanned).
**This new version is recommended for all actual implementations**_



## IMPORTANT ##
This program runs on unix machines only. There is no win32 support planned
(anymore), so if you want the program run on windows machines, port it
yourself - it's open source! ;)
For now, not all HTTP codes are implemented and for some a UNKNOWN will be
returned. Full implementation of HTTP codes will be implemented, soon.

## LICENSE ##
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