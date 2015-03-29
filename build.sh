#!/bin/bash
# what even is a computer
export C_INCLUDE_PATH="/usr/local/include/taglib"
export CPLUS_INCLUDE_PATH="/usr/local/include/taglib"

if [ "$1" != "--quick" ]; then
# .
    phpize
    ./configure --with-php-config=/usr/local/bin/php-config --enable-debug --with-taglib
fi
env CXXFLAGS="$CXXFLAGS -std=c+11"

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
                echo -ne "\033[1;32m $test succeeded!\033[0m\n"
                if [ "$2" == "--yes-tests" ]; then
                    cat /tmp/q
                fi
            else
                echo -ne "\033[0;31m :: $test FAILED :: \033[0m\n"
                cat /tmp/q | less
                break
            fi
        done
    fi
else
    cat /tmp/q | less
fi
