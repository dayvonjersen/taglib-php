#!/bin/bash
# what even is a computer
export C_INCLUDE_PATH="/usr/local/include/taglib"
export CPLUS_INCLUDE_PATH="/usr/local/include/taglib"

if [ "$1" != "--quick" ]; then
# .
    phpize --with-php-config=/etc/php5/fpm/php.ini   
    ./configure --with-php-config=/usr/php5/fpm/php.ini --enable-debug --with-taglib
    sed -i.bak 's/\(^CXXFLAGS =\)/\1 -std=c++11/' Makefile
    if [[ $? = 1 ]]; then 
        sed -i .bak 's/\(^CXXFLAGS =\)/\1 -std=c++11/' Makefile
    fi;
    rm Makefile.bak
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
        cd test
        php -f taglib-php-tests.php | less -r
    fi
else
    cat /tmp/q | less
fi
