#!/bin/bah

#Installation script for UBUNTU

RUBY_INSTALLED=$(which ruby)
if [ -z "${RUBY_VERSION// }" ]
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

    #installing ruby
    rvm install 2.3
    rvm use 2.3
fi

#Installation and configuration steps for haikunet

#Add directory to $PATH
HAIKUNET_DIRECTORY=$(pwd)
echo "export PATH=\"$PATH:$HAIKUNET_DIRECTORY/lib\" # Adding Haikunet to your PATH variable" >> ~/.bashrc

#Install gems 
gem install bundler

#Second, we install bundler if not installed
#BUNDLER_INSTALLED=$(gem list -i bundler)
#if [[ ! $BUNDLER_INSTALLED ]]; then IS NOT WORKING, IT RETURNS TRUE BESIDES BUNDLER IS NOT INSTALLED
#	gem install bundler
#fi

#Finally, we install all gems
bundler install