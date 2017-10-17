#!/bin/bash

# Simple script for adding contributions.

# remove dangling files
make distclean
make clean

# add all new files
git add ./

# commit revision
git commit

# push to remote
git push origin master

