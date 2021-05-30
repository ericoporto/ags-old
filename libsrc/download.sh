#!/bin/bash

set -e

# change to directory where script is located
cd "$( dirname "${BASH_SOURCE[0]}" )"

function get {
    local URL=$1
    local FILENAME=$2
    if [[ ! -f $FILENAME ]]; then
        echo
        echo ${FILENAME}: $URL
        echo
        curl -L $URL --output ${FILENAME}
    fi
}

LIBOGG_VERSION=1.3.4
get https://github.com/xiph/ogg/releases/download/v${LIBOGG_VERSION}/libogg-${LIBOGG_VERSION}.tar.gz libogg-${LIBOGG_VERSION}.tar.gz
get https://downloads.xiph.org/releases/ogg/libogg-${LIBOGG_VERSION}.tar.gz libogg-${LIBOGG_VERSION}.tar.gz

LIBVORBIS_VERSION=1.3.7
get https://github.com/xiph/vorbis/releases/download/v${LIBVORBIS_VERSION}/libvorbis-${LIBVORBIS_VERSION}.tar.gz libvorbis-${LIBVORBIS_VERSION}.tar.gz
get http://downloads.xiph.org/releases/vorbis/libvorbis-${LIBVORBIS_VERSION}.tar.gz libvorbis-${LIBVORBIS_VERSION}.tar.gz

LIBTHEORA_VERSION=1.1.1
get https://github.com/xiph/theora/archive/refs/tags/v${LIBTHEORA_VERSION}.tar.gz libtheora-${LIBTHEORA_VERSION}.tar.gz
# get https://downloads.xiph.org/releases/theora/libtheora-${LIBTHEORA_VERSION}.tar.bz2 libtheora-${LIBTHEORA_VERSION}.tar.bz2

#new work in svn
#svn co http://svn.xiph.org/trunk/Tremor/
#git clone https://git.xiph.org/tremor.git
get https://s3-ap-southeast-2.amazonaws.com/ags-shared/libtremor-20150108-r19427.tar.bz2 libtremor-20150108-r19427.tar.bz2

get https://s3-ap-southeast-2.amazonaws.com/ags-shared/lua-5.1.5.tar.gz lua-5.1.5.tar.gz
get http://www.lua.org/ftp/lua-5.1.5.tar.gz lua-5.1.5.tar.gz

shasum --check sha1sums
