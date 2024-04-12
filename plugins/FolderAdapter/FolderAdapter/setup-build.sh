#!/bin/bash -e

if [[ "$USER" != "" ]]; then
	# Install tools required for compiling, and system libs.
	echo "sudo apt-get"
	sudo apt update -q
	sudo apt install build-essential -y -q
	sudo apt install openssl -y -q
	sudo apt install libssl-dev -y -q
	sudo apt install zlib1g zlib1g-dev -y -q
	sudo apt install wget -y -q
	sudo apt install zip -y -q
	sudo apt install unzip -y -q
	sudo apt install p7zip-full p7zip-rar -y -q
	sudo apt install cmake -y -q
	sudo apt install git -y -q
else
	# Install tools required for compiling, and system libs.
	echo "apt-get"
	apt-get update -q
	apt-get install build-essential -y -q
	apt-get install openssl -y -q
	apt-get install libssl-dev -y -q
	apt-get install zlib1g zlib1g-dev -y -q
	apt-get install wget -y -q
	apt-get install zip -y -q
	apt-get install unzip -y -q
	apt-get install p7zip-full p7zip-rar -y -q
	apt-get install cmake -y -q
	apt-get install git -y -q
fi