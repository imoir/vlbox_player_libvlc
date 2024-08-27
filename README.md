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

## Required installations
- Open a terminal command window
- ```sudo apt update```
- ```sudo apt upgrade```
- ```sudo apt-get install git build-essential autoconf automake libtool```
- ```sudo apt-get install libvlc-dev```

## Get and Build player
- ```cd ~```
- ```git clone git@github.com:imoir/vlbox_player_libvlc.git```
- ```cd vlbox_player_libvlc/src```
- ```make```

## Prepare media for player
- create a ```~/media``` directory
  - copy the contents of vlbox_player_libvlc/misc/media here
  - also place other media files here (no spaces in the filenames preferred)
- modify the file vlbox_player_libvlc/misc/config/config.conf as required for paths

## Execute player from ssh
- connect to device using ssh
- config file location, modify for path ```export VLBOX_CONFIGURATION="/home/intenscity/vlbox_player_libvlc/misc/config/config.conf"```
- a user session on XWindows must be present.
- set display with ```export DISPLAY=:0.0```
- ```~/vlbox_player_libvlc/bin/player & disown```
- press enter to see the prompt

To control player, do the following at the prompt
- ```echo play:video.mp4 >> /tmp/named.pipe```
- ```echo play:rainbow.jpg >> /tmp/named.pipe```
- ```echo stop >> /tmp/named.pipe```
- ```echo quit >> /tmp/named.pipe```

## libVlc

- ```git clone https://github.com/videolan/libvlcpp.git```

# Device Setup

## Rapsberry Pi 5 (rpi5)
- download the Raspberry Pi Imager from https://www.raspberrypi.com/software/ and install
- download the image 2024-07-04-raspios-bookworm-arm64.img.xz
  - from https://downloads.raspberrypi.com/raspios_arm64/images/raspios_arm64-2024-07-04/
- insert microSD card in computer, run imager and image card with image file
  - in disk imager, for ```Operating System```, select ```Use custom``` from drop down box, then select the image file
  - at the ```Use OS customization?```, click on the ```EDIT SETTINGS``` button
  - in the ```GENERAL``` tab, check the ```Set username and password``` checkbox
  - enter  ```intenscity``` as the ```Username:```, and enter the usual password
  - in the ```SERVICES``` tab, check the ```Enable SSH``` checkbox, and select the ```Use password authentication``` radio button
  - click the ```SAVE``` button, then click the ```YES``` button to apply the customisation settings
- insert card in rpi5 and turn on
- once booted up, open a terminal window
- ```sudo apt update```
- ```sudo apt upgrade```
- copy .ssh directory to ```/home/intenscity/```
- ```chmod 700 .ssh```
- ```chmod 600 .ssh/id_ed25519```
- ```mkdir -p prod/scenarios/test```
- ```cd prod```
- ```git clone git@github.com:imoir/vlbox_player_libvlc.git player```
- ```sudo apt-get install libvlc-dev```
- ```cp player/misc/media/black.jpg scenarios/```
- ```cp player/misc/media/rainbow.jpg scenarios/test/```
- ```cp player/misc/config/vlbox.conf .```
- ```cd player/src```
- ```make```
- copy any other videos to the prod/scenarios/test directory
- execute the script prod/player/misc/scripts/runPlayer.sh

from an ssh terminal, send commands to the player as follows:
- ```echo play:test/rainbow.jpg >> /tmp/player-manager```
- ```echo play:test/video.mp4 >> /tmp/player-manager```
- ```echo stop >> /tmp/player-manager```
- ```echo quit >> /tmp/player-manager```

### Alternative image
- download the image 2024-07-04-raspios-bookworm-arm64-full.img.xz
  - from https://downloads.raspberrypi.com/raspios_full_arm64/images/raspios_full_arm64-2024-07-04/

## Device setup for system testing

### Install and run manager
- Edit /home/intenscity/prod/vlbox.conf for the correct id and name (e.g. 66c8e979b4af067d0eced8ff and VLBox0011-imo)
- ```curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash```
- ```source ~/.bashrc```
- ```nvm install 18```
- ```npm install --global yarn```
- ```npm install --global forever```
- ```cd ~/prod```
- ```mkdir logs```
- ```git clone git@github.com:imoir/intenscity-vlbox-manager.git manager```
- ```cd manager```
- ```git checkout libvlc```
- ```yarn```
- ```yarn build```
- ```yarn staging```
- verify that vlbox-manager is running using the command ```forever list```

### Set player to run as service
Copy the following into the file ```/etc/systemd/system/intenscity-player.service```
```
[Unit]
Description=Intenscity Player

[Install]
WantedBy=multi-user.target

[Service]
ExecStart=/home/intenscity/prod/player/misc/scripts/runPlayer.sh
Type=simple
User=intenscity
Group=intenscity
WorkingDirectory=/home/intenscity/prod/player/bin
Restart=on-failure
```

Then start the player service with the command ```sudo systemctl enable intenscity-player.service ```

### Set manager to run at startup

Add the following to the end of the file `/home/intenscity/.bashrc` :
```
VLBOX_DIR=/home/intenscity/prod/manager
pushd $VLBOX_DIR > /dev/null
managerLines=`forever list 2> /dev/null | grep 'Intenscity - vlbox-manager' | wc -l`
stoppedLines=`forever list 2> /dev/null | grep 'Intenscity - vlbox-manager' | grep 'STOPPED' | wc -l`
managerRun=$(($managerLines - $stoppedLines))
if [ $managerRun = 0 ]
then
   yarn staging > /dev/null 2>&1
fi
popd > /dev/null
```

### Back office setup
- enter device info in back office : bo.intenscity.io
  - create account:
    - Name : imoTest
  - create user:
    - Email : ian.moir@tlmgo.com
    - Firstname : Ian
    - Lastname : Moir
  - add user to account ??
  - create building:
    - Account Id : imoTest
    - Name : imoSousSol
    - Address :  Ave. Sasseville, Quebec, QC, Canada G1W 1A1
    - Lat : 46.766411739273906
    - Lng : -71.28644382709498
  - create a scenario
    - Version : 1
    - Name : imoScene
    - Type : Simple
    - Duration : 45
    - Color : green
    - Building Id : imoSousSol
    - Status : Available
  - create VLBox:
    - Id : 66c8e979b4af067d0eced8ff
    - Type : Vlbox
    - Name : VLBox0011-imo
    - Account Id : imoTest
    - Building Id : imoSousSol
    - Brand : Intenscity
    - Model : Pi5-v1
    - Mode : Video
    - Light Identifier : Dina 1A
  - copy the VLBox id to the id field in the file prod/vlbox.conf
    - not strictly needed, but do the same with the name


## Make a disk image
An image can be made for distribution to other rpi5 devices from a windows machine.
Note that win32diskimager will amke an initial image of the whole microSD card so using a smaller card (e.g. 16gb) is a good idea.
- remove files that are not required and should not be shared
  - ssh keys
  - logs
  - history
- shutdown rpi5
- remove microSD card
- in Windows, install then run win32diskimager - https://sourceforge.net/projects/win32diskimager/
- insert the microSD card into windows
- in win32diskimager
  - select the microSD card in the ```Device``` drop down
  - for ```Image File```, enter a filename to write the image to, e.g. C:/intenscity/vlbox_img/vlbox_dev_240821.img
  - click on the ```Read``` button
- ensure Windows Subsystem for Linux (WSL) is installed
- open a WSL terminal window (click ```start`` and search for WSL)
- execute the following commands
  - cd to the directory with the image, e.g. cd /mnt/c/intenscity/vlbox_img/
  - git clone https://github.com/Drewsif/PiShrink.git
  - sudo PiShrink/pishrink.sh -z vlbox_dev_240821.img
- the resulting file can be imaged to a microSD card and run on an rpi5
  - the imaging can be done with either win32diskimager (recommended) or the Raspberry Pi imager

## Orange Pi 5 (opi5)

Material
- NanoPi R6S
- microSD card (ScanDisk 64gb)

Procedure
- microSD to eMMC method used
- on Windows, download and extract image file
  - file : rk3588-eflasher-debian-bookworm-core-6.1-arm64-20240522.img.gz (Debian 12)
  - from : https://drive.google.com/drive/folders/1NjpFPnlZua0APcRv4itt6kQzYX0143Jh
- on Windows, download and extract disk imager
  - file : win32diskimager.rar
  - from : https://drive.google.com/drive/folders/1Usla7kNUYVzbMFYN_Aey9NUqgUfXwvqR
- insert SD card into windows machine
- use the disk imager run in administrator mode to image the SD card  
- remove SD card from windows machine and insert into NanoPi and turn on NanoPi
- once installed, remove
- ```sudo apt-get update```
- ```sudo apt-get install task-gnome-desktop``` - long install, at least 60 minutes
- restart with ```sudo shutdown --reboot now```
- ```sudo apt-get install vlc```

Testing with vlc showed that video accelration was not working in this case, out of the box.

Testing with the rk3588-eflasher-debian-bullseye-desktop-6.1-arm64-20240620.img.gz image (Debian 11)
indicates that hardware acceleration works with the default video player.
No installation for vlc was available.

### Useful Links
- https://www.friendlyelec.com/index.php?route=product/product&product_id=289
- https://wiki.friendlyelec.com/wiki/index.php/NanoPi_R6S
- https://drive.google.com/drive/folders/1KnmBQ3Z0M_5snRC24LjhKb8_tKbcfOkw
- http://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/service-and-support/Orange-Pi-5-Pro.html
