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
    if [ "$2" != "--no-tests" ]; then
        for test in $(ls test); do 
            php -f test/$test > /tmp/q;
            if [ "$?" -eq 0 ]; then
                echo -ne "\033[1;32m $test succeeded!\033[0m"
                if [ "$2" == "--yes-tests" ]; then
                    cat /tmp/q
                fi
            else
                echo -ne "\033[0;31m :: $test FAILED :: \033[0m"
                cat /tmp/q | less
            fi
        done
    fi
else
    cat /tmp/q | less
fi
