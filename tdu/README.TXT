Sorry for my bad english, feel free to send corrections :-)

This Project is in absolutely beginning state. It's alpha state, so use with caution. But it's usable so far.

Installation:
 
Either use the precompiled buildufile binary or compile your own. Delete the included and use make (Makefile is present) to compile a new one.
I do only have Linux and some x86 machines. I use original du/coreutils sources, which runs at all kind of machines, but I've omited some parts I didn't understand.
Maybe it works at other OSes/architectures. Please try and send some feedback.

Then create a Database file with "buildufile [path] [dbfile]"
[path] - path to directory to scan (e.g. /, /home, ../log, ...)
[dbfile] - database file (e.g. /var/www/htdocs/dbfile)

The dbfile must be accessible by the web-scrips (php).

Copy all files below "web" to your web space.
Edit tdu-config.php.inc to your needs. (change language, location of dbfile)

Thats it. Open a web-browser and enter your tdu's address.

If you want to use tdu in regulary intervalls at big servers (e.g. Terrabyte NAS environments), use cron to scan directorys at night.

