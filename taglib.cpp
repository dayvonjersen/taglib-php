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

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_vm.h"

/**
 * taglib reports errors through std::cerr
 * let's expose these messages to PHP 
 * so they can actually be detected and handled */
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

//      zend_throw_exception_ex(taglib_exception, 0 TSRMLS_CC, errorMessage);
    if(taglib_cerr.peek() == 'T')
    {
        char fgsfds[255];
        taglib_cerr.getline(fgsfds,255);
        php_error(E_WARNING, "%s", fgsfds);
        retval = true;
    }
//    zend_replace_error_handling( EH_NORMAL, NULL, NULL TSRMLS_CC);
    return retval;
}

#include "taglibmpeg.cpp"
