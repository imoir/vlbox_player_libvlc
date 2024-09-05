#!/bin/bash
sudo apt-get install apt-transport-https gnupg -y
wget -qO - https://download.packetriot.com/linux/debian/pubkey.gpg | sudo apt-key add -
echo "
deb [arch=amd64] https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-amd64 /
deb [arch=i386]  https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-i386  /
deb [arch=armhf] https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-armhf /
deb [arch=arm64] https://download.packetriot.com/linux/debian/buster/stable/non-free/binary-arm64 /
" | sudo tee /etc/apt/sources.list.d/packetriot.list
sudo apt-get update
sudo apt-get install pktriot
sudo -u pktriot pktriot configure
sudo -u pktriot pktriot tunnel tcp forward --destination 127.0.0.1 --dstport 22
sudo systemctl enable pktriot
