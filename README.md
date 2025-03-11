# A shell menu utility for Linux

# Installation for Debian
## Get shmenu and install it
Get the appropriate deb package for your architecture. Check the [releases](https://github.com/owenduffy/shmenu/releases) and use the
appropriate file name below.

If you are using 64 bit RPi (arch=AArch64 or AMD64) use the arhf deb package and see the notes later.

#get shmenu package and install it  
wget https://github.com/owenduffy/shmenu/releases/download/v2.8.0/shmenu-2.10.0-1-armhf.deb  
sudo dpkg -i shmenu-10.7.0-1-armhf.deb

Type shmenu at the prompt and you should get the shmenu copyright message.

## libncurses missing?

If you are missing the libncurses dependency

### arch=AArch64 (RPi with 64bit OS)

#get and install libncursesw as necessary  
sudo apt-get install libncursesw6:armhf

### other architectures (including 64bit RPi with 32bit OS)

#get and install libncurses6 as necessary  
sudo apt-get install libncursesw6

## EDITOR

The script uses the environment variable EDITOR. If it is not set sensibly then:

- find the path to your preferred editor, eg whereis nano
- at the end of ~/.profile, add this line (modified to suit your own editor)

export EDITOR=/usr/bin/nano

Log off and login again to test it.

# Unified characters

v2.11 uses libncursesw and should support UTF-8.
