# Explanation
The idea is to provide a VERY EASY platform, to develop embebed HTML5 apps/systems. The idea is to provide a system based on GNU/Linux, X System and Webkit Web Browser with an extended JavaScript API that allows to connect to a Python service that will extend JavaScript API, allowing to run extra functionalities or specific drivers for several embebed systems like infopoints or ticketing machines.

# Autologin
The idea is to launch X on a GNU/Linux based system and then run ArinOs Web Browser and the Python service (not added to the repository yet). To make it automatically you can follow this steps:

Edit /etc/inittab

	#vi /etc/inittab
	#nano /etc/inittab

Search this line and disable it

	#1:2345:respawn:/sbin/getty 38400 tty1

Add the following line, where YOUR_USER_NAME is the user is going to autologin

	1:2345:respawn:/bin/login -f YOUR_USER_NAME tty1 </dev/tty1 >/dev/tty1 2>&1

Edit .bash_profile on autologin user HOME

	#vi ~/.bash_profile
	#nano ~/.bash_profile

And add this script to autostart X system and ArinOs

	if [ -z "$DISPLAY" ] && [ $(tty) = /dev/tty1 ]; then
	startx
	[path_to_arinos]/arinos
	fi
