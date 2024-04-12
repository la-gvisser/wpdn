#!/bin/bash -e

pwd
cat /proc/sys/kernel/hostname

git config --global http.sslVerify false

echo '********** env **********'
env

cd /home/swuser/GitLab/FolderAdapter
sudo apt update
sudo apt install lsb-release -y
sudo apt install build-essential -y
sudo apt install libpcap-dev -y
sudo apt install openssl -y
sudo apt install libssl-dev -y
sudo apt install zlib1g zlib1g-dev -y
sudo apt install wget -y
sudo apt install zip -y
sudo apt install unzip -y
sudo apt install p7zip-full p7zip-rar -y
sudo apt install git -y

echo '********** lsb-release **********'
cat /etc/lsb-release
osversion=$(lsb_release -rs | sed 's/\./_/g')

export USER=swuser
echo $USER

echo 'building FolderAdapter for Ubuntu $osversion '

/bin/bash build_$osversion.sh

echo 'Jenkins stopped'