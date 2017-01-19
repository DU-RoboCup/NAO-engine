#!/bin/bash

if [ "$1" == "local" ] || [ "$2" == "local" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    cmake ../ -DUSE_CROSS=NO
    make
fi

if [  "$1" == "cross" ] || [ "$2" == "cross" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    cmake ../ -DUSE_CROSS=YES
    make
    cd /home/vagrant/NAO/NAO-engine/naoqi_modules/halagent/
    qibuild configure
    qibuild make
    mkdir -p /home/vagrant/NAO/NAO-engine/build-cross/naoqi_modules
    cp /home/vagrant/NAO/NAO-engine/naoqi_modules/halagent/build-linux64/sdk/bin/hal_experimental /home/vagrant/NAO/NAO-engine/build-cross/naoqi_modules
fi

cd /home/vagrant/NAO/NAO-engine/
