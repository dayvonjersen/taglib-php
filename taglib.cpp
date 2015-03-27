/**
 * TagLib implementation and php extension
 * Mostly for manipulating Tags
 * But also reading audioProperties
 * Aiming to implement MPEG (MP3), FLAC, and OGG */

/**
 * standard libs */
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <sstream>

/**
 * .h for this file (taglib.cpp) */
#include "php_taglib.h"

/**
 * taglib reports errors through std::cerr
 * let's expose these messages to PHP 
 * so they can actually be detected and handled */

/**
 * This idea came from a post I found on Google Groups
 * posted August 15, 2002 by Chris Uzdavinis
 * https://groups.google.com/d/msg/borland.public.cppbuilder.language/Uua6t3VhELA/vGyq-ituxN8J
 */
static std::stringstream taglib_cerr;
class Stream_Swapper {
public:
    Stream_Swapper(std::ostream &orig, std::ostream &replacement) : buf_(orig.rdbuf()), str_(orig)
    {
        orig.rdbuf(replacement.rdbuf());
    }
    ~Stream_Swapper()
    {
        str_.rdbuf(buf_);
    }
private:
    std::streambuf *buf_;
    std::ostream &str_;
} swapper(std::cerr, taglib_cerr);

static bool taglib_error()
{
    bool retval = false;
/**
 * XXX I would like to use exceptions instead of php errors
 * XXX but I get a linker error when I try to do that
 * XXX and php_set_error_handling() is depreciated in favor of this
 * XXX in fact, it's an inline function to call zend_replace_error_handling
 *
 * if you have any insight into this matter, please email me [tso@teknik.io] */
//  static zend_class_entry *taglib_exception;
//  zend_replace_error_handling( EH_THROW, taglib_exception, NULL TSRMLS_CC );
//  zend_throw_exception_ex(taglib_exception, 0 TSRMLS_CC, errorMessage);

    // all TagLib errors happen to be prefixed with either
    // "TagLib: " - debug() - tdebug.cpp
    // "*** " - debugData() - tdebug.cpp
    // and std::cerr sometimes contains more than just '\0'

    if(taglib_cerr.peek() == 'T' || taglib_cerr.peek() == '*')
    {
        char errorMessage[255];
        taglib_cerr.getline(errorMessage,255);
        php_error(E_WARNING, "%s", errorMessage);
        retval = true;
    }
//    zend_replace_error_handling( EH_NORMAL, NULL, NULL TSRMLS_CC);
    return retval;
}

/**
 * baby duck syndrome */
constexpr unsigned int _charArrForSwitch(const char* str, int index = 0)
{
    return !str[index] ? 0x1505 : (_charArrForSwitch(str, index + 1) * 0x21) ^ str[index];
}

constexpr unsigned int operator"" _CASE ( const char str[], size_t size )
{
    return _charArrForSwitch(str);
}

#include "taglibmpeg.cpp"
