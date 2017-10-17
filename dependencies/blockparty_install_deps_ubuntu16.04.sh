#!/bin/bash

# ####################################################
# ### Blockparty Dependency Installer ################
# ####################################################
#
# This simple script is designed to install the packages
# required for blockparty apps to compile.
#
# This script is confirmed working for ubuntu 16.04 as of 10/15/2017.  It may break in the future if unknown changes occur.
#
# Current Dependencies:
# - libevent (not our code, but some deps depend on this)
# - libxml2
# - libssl
# - libcurl
# - libvmime (tenative for removal)
# - libpcap
# - libmailutils
# - libglib
# - libjson
# - libstatgrab
# - libmariadbclient
# - libmicrohttpd


echo " >> installing blockparty dependencies from apt directly, where available."

# install gcc/g++ make and cmake
apt-get install -y gcc g++ make cmake

# install libevent
apt-get install libevent-dev

# install supplemental package
apt-get install -y libxml2-dev libssl-dev libcurl4-openssl-dev libvmime-dev libpcap-dev libmailutils-dev libglib2.0-dev libjson-glib-dev libstatgrab-dev libmariadbclient-dev libmicrohttpd-dev libmariadb-client-lgpl-dev



# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# %%% LIBCSV %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

# install csv parser
echo " >> Attempting to install libcsv_parser from source."
pushd ./
cd ./dependencies/libcsv_parser++-1.0.0/
./configure
make clean
make
make install
popd

# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# %%% Libmicrohttpd (w/ websocket support) %%%%%%%
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

echo " >> Attempting to install libmicrohttpd from source."
pushd ./
cd ./dependencies/libmicrohttpd/
./BuildAndInstallBlockparty.sh
popd


# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# %%% LIBMOZJS (Spidermonkey) %%%%%%%%%%%%%%%%%%%%
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


echo "[+] Now you must also install mozjs31.  This is not included by default in ubuntu"
echo "    so you'll need to either build it from source (not recommended) or use the "
echo "    following ppa:"
echo "  add-apt-repository ppa:ricotz/testing"
echo "  apt-get update "
echo "  apt-get install libmozjs31-dev"


# verbosity
echo "[+] Script terminating."
