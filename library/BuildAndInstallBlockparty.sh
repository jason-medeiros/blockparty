#!/bin/bash

# Simple script to install blockparty.  The library should install 
# to the following positions on disk:
#    library:   /usr/local/lib/libblockparty.a
#    includes:  /usr/local/include/blockparty/include/
#
# If your system is setup differently, autotools may install
# the libraries to different positions, so be aware of your
# own system outputs.  Usually you can simply run:
#
#  find / | grep -i libblockparty.a 
#
# to find the lib and
#
#  find / | grep -i BP-Main.h
#
# to find the headers.

# reconfigure 
autoreconf -fi

# configure build
./configure

# build the library (assuming 8 cores)
make -j 8

# install the library and headers
make install
