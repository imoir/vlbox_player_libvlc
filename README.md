# Development Environment

The instructions have been confirmed on the following platform:
- rpi5 (Raspeberry Pi 5) Debian 12 (bookworm)

## Install and launch ssh server
- ```sudo apt update```
- ```sudo apt-get install openssh-server net-tools```
- ensure the ssh server is running using : ```sudo systemctl status ssh```
- if it is not running, try this then check again : ```sudo systemctl enable --now ssh```
- get the ip address with : ```ifconfig```

## Recommended tools

- Meld - Compare directory trees - Install with ```sudo apt-get install meld```

### xrdp - Remote Desktop Protocol - Microsoft's remote desktop application

Remote desktop is useful for remotely working on the rpi5.

There are issues with GPU use by some applications while using xrdp.
These include firefox and chromium.

To get firefox working in xrdp, invoke firefox in safe mode from a terminal window: ```firefox --safe-mode```.
- Then go to ```Settings|General|Performance```
- Uncheck the ```Use recommended performance settings``` checkbox
- Uncheck the ```Use hardware acceleration when available``` checkbox
firefox will run without using the GPU at the next startup.

### vscode - Visual Studio Code - IDE (Integrated Development Environment)

Install with ```sudo apt install code```

Once Visual Studio Code is executed, do a ```<ctrl><shift>X``` to open the extension side panel.
Look for and install the following extensions:
- C/C++

Recomended extensions:
- Git Graph

TODO: is the ```C/C++ Extension Pack``` extension with the CMake and CMake tools useful

code suffers from the same GPU issues as firefox does in xrdp.
To invoke code without the GPU issues, use ```code --disable-gpu``` from the command line.
To always invoke without GPU, do a ```<ctrl><shift>P```, find and select the ```Preferences: Configure Runtime Arguments``` command, then in the argv.json file uncomment or add the following line, and then restart code:
- ```"disable-hardware-acceleration": true```

# Build and Execute player
The following instructions prepare a device for compiling and running the player code.

Required installations
- Open a terminal command window
- ```sudo apt update```
- ```sudo apt upgrade```
- ```sudo apt-get install git build-essential autoconf automake libtool```
- ```sudo apt-get install libvlc-dev```

possibly:
- ```sudo apt-get install apt-file```

Get and Build player
- ```cd ~```
- ```git clone git@github.com:imoir/vlbox_player_libvlc.git```
- ```cd vlbox_player_libvlc/src```
- ```make```

Execute player
- create a ```~/media``` directory
  - copy the contents of vlbox_player_libvlc/misc/media here
  - also place other media files here (no spaces in the filenames preferred)
- modify the file vlbox_player_libvlc/misc/config/config.conf as required for paths
- config file location, modify for path ```export VLBOX_CONFIGURATION="/home/pi/vlbox_player_libvlc/misc/config/config.conf"```
- A user session on XWindows must be present.
  - If running from ssh, still required, set display with ```export DISPLAY=:0.0```
- ```~/vlbox_player_libvlc/bin/player```

To control player, do the following from another ssh session
- ```echo play:video.mp4 >> /tmp/named.pipe```
- ```echo play:rainbow.jpg >> /tmp/named.pipe```
- ```echo stop >> /tmp/named.pipe```
- ```echo quit >> /tmp/named.pipe```

## libVlc

- ```git clone https://github.com/videolan/libvlcpp.git```
