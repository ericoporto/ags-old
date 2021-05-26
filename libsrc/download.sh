#!/bin/bash

set -e

# change to directory where script is located
cd "$( dirname "${BASH_SOURCE[0]}" )"

function get {
    local URL=$1
    local FILENAME=$(basename $URL)
    if [[ ! -f $FILENAME ]]; then
        echo
        echo ${FILENAME}: $URL
        echo
        curl -O $URL
    fi
}

get https://s3-ap-southeast-2.amazonaws.com/ags-shared/libogg-1.3.2.tar.gz
get http://downloads.xiph.org/releases/ogg/libogg-1.3.2.tar.gz

get https://s3-ap-southeast-2.amazonaws.com/ags-shared/libvorbis-1.3.5.tar.gz
get http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.gz

#git repo is definitely where new work happens
#git clone https://git.xiph.org/theora.git
get https://s3-ap-southeast-2.amazonaws.com/ags-shared/libtheora-20150828-gfbb2758.tar.bz2
get https://s3-ap-southeast-2.amazonaws.com/ags-shared/libtheora-20160525-g50df933.tar.bz2

#new work in svn
#svn co http://svn.xiph.org/trunk/Tremor/
#git clone https://git.xiph.org/tremor.git
# get https://s3-ap-southeast-2.amazonaws.com/ags-shared/libtremor-20150108-r19427.tar.bz2

# get https://s3-ap-southeast-2.amazonaws.com/ags-shared/lua-5.1.5.tar.gz
# get http://www.lua.org/ftp/lua-5.1.5.tar.gz

shasum --check sha1sums
