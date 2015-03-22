PHP_ARG_ENABLE(taglib, 
    [Whether to enable the "taglib" extension], 
    [ --enable-taglib Enable "taglib" extension support])

if test $PHP_TAGLIB != "no"; then
        PHP_REQUIRE_CXX
        AC_LANG_CPLUSPLUS 
         
        PHP_ADD_LIBRARY(stdc++, 1, TAGLIB_SHARED_LIBADD)

        PHP_SUBST(TAGLIB_SHARED_LIBADD)
        PHP_NEW_EXTENSION(taglib, taglib.c, $ext_shared)
fi
