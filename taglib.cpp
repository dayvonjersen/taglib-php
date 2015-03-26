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

    std::streambuf *error_stream_buffer = std::cerr.rdbuf();
    char *errorMessage;
    int i = 0;
    while(error_stream_buffer->snextc() != EOF)
    {
        char c = error_stream_buffer->sgetc();
        strcat(errorMessage,&c);
        i++;
    }

    if(i)
    {
//      zend_throw_exception_ex(taglib_exception, 0 TSRMLS_CC, errorMessage);
        php_error(E_WARNING, "%s", "testing something");
        retval = true;
    }
//    zend_replace_error_handling( EH_NORMAL, NULL, NULL TSRMLS_CC);
    return retval;
}

#include "taglibmpeg.cpp"
