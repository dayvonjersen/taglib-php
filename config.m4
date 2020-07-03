# Since we require an external lib we use --with instead of --enable
PHP_ARG_ENABLE(taglib, 
    [   with-taglib     Include taglib support])

if test $PHP_TAGLIB != "no"; then
        # need to compile with C++
        PHP_REQUIRE_CXX
        AC_LANG_CPLUSPLUS 
         
        # and the standard libraries
        PHP_ADD_LIBRARY(stdc++, 1, TAGLIB_SHARED_LIBADD)

        # but we also need libtag.so so there's this strange macro
        # this is just a random symbol from libtag
        PHP_CHECK_LIBRARY(tag, _ZN10ByteReaderD0Ev, 
        [ 
            PHP_ADD_LIBRARY_WITH_PATH(tag, $(taglib-config --prefix), TAGLIB_SHARED_LIBADD)
            AC_DEFINE(HAVE_TAGLIB, 1, [ ])
        ], [
            AC_MSG_ERROR([libtag.so not found! Make sure you have installed (taglib.github.io) and libtag.so is in /usr/local/lib (and the .h files are in /usr/local/include/taglib)])
        ], [
            $(taglib-config --libs)
        ])
        # this line twice because I don't know what I'm doing
        $(taglib-config --libs)

        # where the taglib .h files live
        PHP_ADD_INCLUDE($(taglib-config --cflags | sed 's/-I//' | sed 's/\/taglib$//'))

        # and some magic incantations
        PHP_SUBST(TAGLIB_SHARED_LIBADD)
        PHP_NEW_EXTENSION(taglib, taglib.cpp, $ext_shared)
	    PHP_ADD_EXTENSION_DEP(standard)
    	PHP_ADD_EXTENSION_DEP(base64)
fi
# m4 is GNU/dumb
