#!/bin/bash
set -e

DIRECTORY="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

OS="`uname`"
case $OS in
  'Linux')
    OS='Linux'
    ;;
  'Darwin') 
    cd $DIRECTORY/submodules
    git submodule update --init --recursive libartnet
    cd $DIRECTORY/submodules/libartnet
    autoreconf -fi
    ./configure --prefix=$DIRECTORY/fakeroot/
    make -j
    make install

    cd $DIRECTORY/submodules
    git submodule update --init --recursive libOSCpp
    cd $DIRECTORY/submodules/libOSCpp
    ./configure --prefix=$DIRECTORY/fakeroot/
    make -j
    make install

    cd $DIRECTORY/submodules
    git submodule update --init --recursive rtmidi
    cd $DIRECTORY/submodules/rtmidi
    ./autogen.sh
    ./configure --prefix=$DIRECTORY/fakeroot/
    make -j
    make install

    cd $DIRECTORY/submodules
    git submodule update --init --recursive restinio
    cd $DIRECTORY/submodules/restinio
    mxxruexternals
  ;;
  *) 
    exit 1
  ;;
esac

