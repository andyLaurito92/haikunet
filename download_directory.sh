#!/bin/bah

WGET_INSTALLED=$(which wget)
if [ -z "${WGET_INSTALLED// }" ]
 then
 	apt-get install wget
fi

wget -L https://github.com/andyLaurito92/haikunet/archive/master.tar.gz ; mkdir -p haikunet && tar xf master.tar.gz -C haikunet --strip-components=1
rm master.tar.gz
rm master.tar.gz.1

cd haikunet
./install_haikunet.sh