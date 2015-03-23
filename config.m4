PHP_ARG_WITH(taglib, 
    [   with-taglib     Include taglib support])

if test $PHP_TAGLIB != "no"; then
        PHP_REQUIRE_CXX
        AC_LANG_CPLUSPLUS 
         
        PHP_ADD_LIBRARY(stdc++, 1, TAGLIB_SHARED_LIBADD)

        PHP_CHECK_LIBRARY(tag, _ZN10ByteReaderD0Ev, 
        [ 
            PHP_ADD_LIBRARY_WITH_PATH(tag, /usr/local/lib, TAGLIB_SHARED_LIBADD)
            AC_DEFINE(HAVE_TAGLIB, 1, [ ])
        ], [
            AC_MSG_ERROR([fuck fuck fuck])
        ], [
            -L/usr/local/lib -ldl
        ])
        PHP_ADD_INCLUDE(/usr/local/include)
        PHP_SUBST(TAGLIB_SHARED_LIBADD)
        PHP_NEW_EXTENSION(taglib, taglib.cpp, $ext_shared)
fi
