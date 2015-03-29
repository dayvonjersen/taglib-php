/**
 * TagLib implementation and php extension
 * Mostly for manipulating Tags
 * But also reading audioProperties
 * Supports MPEG (MP3) and OGG, soon(tm): FLAC */

/**
 * standard libs */
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <sstream>

/**
 * .h required for php extensions */
#include "php_taglib.h"

/**
 * using extensions within extensions */
#include <ext/standard/base64.h>

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
 * if you have any insight into this matter, please email me [tso@teknik.io] 
 * UPDATE[3/28/2015 7:21:50 PM]
 * I think I can just do zend_throw_exception() anywhere. 
 */

//  static zend_class_entry *taglib_exception;
//  zend_replace_error_handling( EH_THROW, taglib_exception, NULL TSRMLS_CC );
//  zend_throw_exception_ex(taglib_exception, 0 TSRMLS_CC, errorMessage);

    /**
     * all TagLib errors happen to be prefixed with either
     * "TagLib: " - debug() - tdebug.cpp
     * "*** " - debugData() - tdebug.cpp
     * and std::cerr sometimes contains more than just '\0' */
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
 * baby duck syndrome - using strings in switch statements
 * 
 * Thanks to Captain Oblivious:
 * http://stackoverflow.com/a/16721551
 *
 * NOTE: This requires C++11. 
 * NOTE: I've manually editted the Makefile to add the -std=c++11 flag
 */
constexpr unsigned int _charArrForSwitch(const char* str, int index = 0)
{
    return !str[index] ? 0x1505 : (_charArrForSwitch(str, index + 1) * 0x21) ^ str[index];
}

constexpr unsigned int operator"" _CASE ( const char str[], size_t size )
{
    return _charArrForSwitch(str);
}

/**
 * making php class constants requires some boilerplate */
#define _defineclassconstant(name, value)           \
    zval * _##name##_ ;                             \
    _##name##_ = (zval *)(pemalloc(sizeof(zval),1));\
    INIT_PZVAL(_##name##_);                         \
    ZVAL_LONG(_##name##_,value);                    \
    zend_hash_add(&ce->constants_table,#name,sizeof(#name),(void *)&_##name##_,sizeof(zval*),NULL);

/**
 * more .h files will be included in each of the .cpp files*/
#include "TSRM.h"
#include <tlist.h>
#include <tpropertymap.h>
#include <tstringlist.h>

/**
 * just trying to separate out some of this code */
#include "taglibmpeg.cpp"
#include "taglibogg.cpp"

/**
 * And let's try to unify them into one extension 
 * which provides all of the classes */
PHP_MINIT_FUNCTION(taglib_minit)
{
    zend_class_entry mpeg_ce;
    zend_class_entry ogg_ce;

    INIT_CLASS_ENTRY(mpeg_ce, "TagLibMPEG", php_taglibmpeg_methods);
    taglibmpeg_class_entry = zend_register_internal_class(&mpeg_ce TSRMLS_CC);
    taglibmpeg_register_constants(taglibmpeg_class_entry);

    taglibmpeg_class_entry->create_object = taglibmpegfile_create_handler;
    memcpy(&taglibmpegfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    taglibmpegfile_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ogg_ce, "TagLibOGG", php_taglibogg_methods);
    taglibogg_class_entry = zend_register_internal_class(&ogg_ce TSRMLS_CC);
    taglibogg_register_constants(taglibogg_class_entry);

    taglibmpeg_class_entry->create_object = tagliboggfile_create_handler;
    memcpy(&tagliboggfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    tagliboggfile_object_handlers.clone_obj = NULL;

    return SUCCESS;
}

zend_module_entry taglib_module_entry = {

#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif

    PHP_TAGLIB_EXTNAME,
    NULL, /* Functions */
    PHP_MINIT(taglib_minit), /* MINIT */
    NULL, /* MSHUTDOWN */
    NULL, /* RINIT */
    NULL, /* RSHUTDOWN */
    NULL, /* MINFO */

#if ZEND_MODULE_API_NO >= 20010901
    PHP_TAGLIB_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TAGLIB
ZEND_GET_MODULE(taglib)
#endif
