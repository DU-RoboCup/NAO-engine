#!/bin/bash

if [ "$1" == "local" ] || [ "$2" == "local" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    rm -rf *
    cmake ../ -DUSE_CROSS=NO
    make
fi

if [  "$1" == "cross" ] || [ "$2" == "cross" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    rm -rf *
    cmake ../ -DUSE_CROSS=YES
    make
fi

cd ..
