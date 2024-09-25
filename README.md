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
- ```sudo apt-get -y install liblog4cpp5-dev liblog4cpp5v5 liblog4cpp-doc```
- ```sudo apt-get -y install nlohmann-json3-dev```

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
- ```cd prod```
- ```git clone git@github.com:imoir/vlbox_player_libvlc.git player```
- ```sudo apt-get install libvlc-dev```
- ```sudo mkdir /etc/vlbox```
- ```sudo chown intenscity:intenscity /etc/vlbox```
- ```cp player/misc/config/vlbox_XXXX.json /etc/vlbox/vlbox.json```
- ```cd player/src```
- ```make```
- execute the script prod/player/misc/scripts/runPlayer.sh

from an ssh terminal, send commands to the player as follows:
- ```echo play:test/rainbow.jpg >> /tmp/player-manager```
- ```echo play:test/video.mp4 >> /tmp/player-manager```
- ```echo stop >> /tmp/player-manager```
- ```echo quit >> /tmp/player-manager```

### Alternative image
- download the image 2024-07-04-raspios-bookworm-arm64-full.img.xz
  - from https://downloads.raspberrypi.com/raspios_full_arm64/images/raspios_full_arm64-2024-07-04/

### Add second sudo user
This will be useful while player is started in .bashrc
- sudo adduser ic2
- sudo usermod -aG sudo ic2

## Device setup for system testing

### Install and run manager
- Edit /etc/vlbox/vlbox.json for the correct id and name (e.g. 66c8e979b4af067d0eced8ff and VLBox0011-imo)
- ```curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash```
- ```source ~/.bashrc```
- ```nvm install 18```
- ```npm install --global yarn```
- ```npm install --global forever```
- ```cd ~/prod```
- ```sudo mkdir /var/log/vlbox```
- ```sudo chown intenscity:intenscity /var/log/vlbox``` 
- ```git clone git@github.com:imoir/intenscity-vlbox-manager.git manager```
- ```cd manager```
- ```git checkout libvlc```
- set the projector ip address, the light box id and address in the /etc/vlbox/vlbox.json file
- ```yarn```
- ```yarn build```
- ```yarn vlbox```
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

Then start the player service with the command ```sudo systemctl enable intenscity-player.service```

### Set player to run in bash session

```
# Only launch player if it is not already running
playerRun=`ps -aux | grep bin/player$ | wc -l`
if [ $playerRun = 0 ]
then
   export DISPLAY=:0.0
   pushd /home/intenscity/prod/player
   bin/player >>/var/log/vlbox/player.log 2>&1
   popd
fi
```

### Set labwc as display manager

sudo raspi-config
select 6 Advanced Options
select A6 Wayland
select W3 Labwc
press enter to accept with red Ok button
press tab until Finish button is selected and press enter
press enter to select red Yes button to reboot

### Set light box configuration

owned_668677d8956eb642534e1147_ip=10.168.0.166
owned_668677d8956eb642534e1147_port=2430
owned_668677d8956eb642534e1147_type=Siudi11A

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
   yarn vlbox > /dev/null 2>&1
fi
popd > /dev/null
```
### Disable unneeded services

sudo apt remove rpi-connect

Add the following the the [all] section in the /boot/firmware/config.txt file
dtoverlay=disable-wifi
dtoverlay=disable-bt

Remove the splash parameter from the file /boot/firmware/cmdline.txt

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
  - copy the VLBox id to the player/id field in the file /etc/vlbox/vlbox.json
    - not strictly needed, but do the same with the name
  - create light box
    - Id : 66cdfa8cb4af067d0eced932
    - Type : Vlbox
    - Name : Light_imo
    - Account Id : imoTest
    - Building Id : imoSousSol
    - Brand : Intenscity
    - Model : VirtualLight
    - Mode : Light
    - Owner Box Id : VLBox0011-imo
    - Light Identifier : Siudi11A

### pktriot installation and setup
- sudo apt-get install apt-transport-https gnupg -y
- wget -qO - https://download.packetriot.com/linux/debian/pubkey.gpg | sudo apt-key add -
-  echo "
deb [arch=amd64] https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-amd64 /
deb [arch=i386]  https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-i386  /
deb [arch=armhf] https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-armhf /
deb [arch=arm64] https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-arm64 /
" | sudo tee /etc/apt/sources.list.d/packetriot.list
- sudo apt-get update
- sudo apt-get install pktriot
- sudo su pktriot
- pktriot configure
- DO NOT under ANY circumstance even ever THINK of trying this command ---> pktriot edit --name 'VLBox0007_imo'
- pktriot tunnel tcp forward --destination 127.0.0.1 --dstport 22
- exit
- sudo systemctl start pktriot
- sudo systemctl enable pktriot

### ssh tunnels

#### Plaza Athénée
ssh -L 12180:192.168.100.100:80 -p22566 pi@quirky-snow-95242.pktriot.net
http://127.0.0.1:12180/cgi-bin/home
http://127.0.0.1:12080/api/v01/control/escvp21?cmd=PWR?

#### Chalon
ssh -L 12807:192.168.10.17:80 -p22884 intenscity@intenscity-0007.pktriot.net
http://127.0.0.1:12807/cgi-bin/home
http://127.0.0.1:12807/api/v01/control/escvp21?cmd=PWR?

#### Bureau Intenscity
ssh -L 12808:10.168.0.183:80 -p22639 intenscity@intenscity-0008.pktriot.net
http://127.0.0.1:12808/cgi-bin/home
http://127.0.0.1:12808/api/v01/control/escvp21?cmd=PWR?

### Varia
- Adjust volume : alsamixer
- Get status from server : https://api.intenscity.io/vlbox-manager/66c8e979b4af067d0eced8ff/status
  - Use VLBox id to get status

## Make a disk image
An image can be made for distribution to other rpi5 devices from a windows machine.
Note that win32diskimager will make an initial image of the whole microSD card so using a smaller card (e.g. 16gb) is a good idea.
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

## Logs
Log files for the manager are found in the directory /var/log/vlbox.
There will be a files per day with files being deleted after 20 days.??

The player logs are integrated into the syslog system.

To view the manager logs in an ssh session, use the ```less``` command.
To view the logs from September 25 2024, use the following command
- less /var/log/vlbox/manager-2024-09-25.log

Initially, the oldest logs are shown first.
Press the letter 'G' (capital g) to move to the latest logs.
From there press, the 'b' key to go back in the log, the space bar to go forward.
Press 'q' to quit.

To view the player logs enter the ```journalctl``` command at the ssh command line. The player logs will be mixed in with the other syslog logs.

To move around the syslog, use the same keys used above when looking at manager logs.

To view logs in real time, use the following commands:
- tail -f /var/log/vlbox/manager-2024-09-25.log
- journalctl -f

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
