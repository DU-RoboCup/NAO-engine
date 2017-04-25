#!/bin/bash

if [ "$1" == "local" ] || [ "$2" == "local" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    rm -rf *
    cmake ../ -DUSE_CROSS=NO
    make -j2
fi

if [  "$1" == "cross" ] || [ "$2" == "cross" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    rm -rf *
    cmake ../ -DUSE_CROSS=YES
    make -j2
    #cd /home/vagrant/NAO/NAO-engine/naoqi_modules/halagent/
    #qibuild configure -c linux64
    #qibuild make -c linux64
    #mkdir -p /home/vagrant/NAO/NAO-engine/build-cross/naoqi/
    #cp /home/vagrant/NAO/NAO-engine/naoqi_modules/halagent/build-crosslinux/sdk/lib/naoqi/* /home/vagrant/NAO/NAO-engine/build-cross/naoqi
fi

cd ..
