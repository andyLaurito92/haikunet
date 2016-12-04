#!/bin/bash

#Installation script for UBUNTU

#We install rvm if not installed
RVM_INSTALLED=$(which rvm)
if [ -z "${RVM_INSTALLED// }" ]
 then
    #installation and configuration of rvm
    gpg --keyserver hkp://keys.gnupg.net --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3

    CURL_VERSION=$(which curl)

    if [ -z "${CURL_VERSION// }" ]
    	then
    	yes | apt-get install curl
    fi

    curl -sSL https://get.rvm.io | bash -s stable --ruby
    echo "export PATH=\"$PATH:$HOME/.rvm/bin\" # Add RVM to PATH for scripting" >> ~/.bashrc
    source /etc/profile.d/rvm.sh
    source /usr/local/rvm/scripts/rvm
    source ~/.bashrc
    rvmsudo rvm get stable 
fi

#installing ruby
rvm install 2.3
rvm use 2.3

#Installation and configuration steps for haikunet

#First we create the binary in order to be run from console
HAIKUNET_DIRECTORY=$(pwd)
if [[ $EUID -ne 0 ]]; then
	sudo ln -s "$HAIKUNET_DIRECTORY/lib/haikunet.rb" /usr/bin/haikunet
else
	ln -s "$HAIKUNET_DIRECTORY/lib/haikunet.rb" /usr/bin/haikunet
fi

gem install bundler

#Second, we install bundler if not installed
#BUNDLER_INSTALLED=$(gem list -i bundler)
#if [[ ! $BUNDLER_INSTALLED ]]; then IS NOT WORKING, IT RETURNS TRUE BESIDES BUNDLER IS NOT INSTALLED
#	gem install bundler
#fi

#Finally, we install all gems
bundler install