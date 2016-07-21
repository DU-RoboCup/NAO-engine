#!/bin/bash

if [ "$0" == "local" ] || [ "$1" == "local" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    rm -rf *
    cmake ../ -DUSE_CROSS=NO
    make
fi

if [  "$0" == "cross" ] || [ "$1" == "cross" ] ; then
    mkdir -p /home/vagrant/NAO/NAO-engine/build
    cd /home/vagrant/NAO/NAO-engine/build
    rm -rf *
    cmake ../ -DUSE_CROSS=YES
    make
fi

cd ..
