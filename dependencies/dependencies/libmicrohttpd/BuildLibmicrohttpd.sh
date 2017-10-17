#!/bin/bash

# Microhttpd is a pain to build.  This script simply makes the
# process into a one-step shell script.  
# ~JM

pushd ./
cd ./libmicrohttpd
autoreconf -fi
./configure
cd ./src/microhttpd
make clean
make
make install
cd ..
cd ./include/
mkdir /usr/include/microhttpd/
cp *.h /usr/include/microhttpd/
popd
