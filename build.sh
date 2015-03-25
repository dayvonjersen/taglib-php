#!/bin/bash
# what even is a computer
export C_INCLUDE_PATH="/usr/local/include/taglib"
export CPLUS_INCLUDE_PATH="/usr/local/include/taglib"

if [ "$1" != "--quick" ]; then
# .
    phpize
    ./configure --with-taglib
fi

# maybe I *should* modify config.m4
# or maybe you should shut up
touch taglib.cpp

make 2> /tmp/q
CATFILE=$(cat /tmp/q)
if [ "$CATFILE" == "" ]; then 
    echo "Build succeeded, copying new module..."
    sudo cp -R modules/* /usr/local/lib/php/modules 
    echo "Hey you, make a test.php and add it to build.sh"
else
    cat /tmp/q | less
fi
