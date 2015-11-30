/**
 * TagLib implementation and php extension
 * Mostly for manipulating Tags
 * But also reading audioProperties
 * Supports MPEG (MP3) and OGG, soon(tm): FLAC
 *
 * taglib.github.io
 * github.com/generaltso/taglib-php
 */

/**
 * standard libs 
 */
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <sstream>

/**
 * .h required for php extensions 
 */
#include "php_taglib.h"

/**
 * taglib reports errors through std::cerr
 * let's expose these messages to PHP 
 * so they can actually be detected and handled 
 *
 * This idea came from a post I found on Google Groups
 * posted August 15, 2002 by Chris Uzdavinis
 * https://groups.google.com/d/msg/borland.public.cppbuilder.language/Uua6t3VhELA/vGyq-ituxN8J
 */
static std::stringstream taglib_cerr;
class Stream_Swapper {
public:
    Stream_Swapper(std::ostream &orig, std::ostream &replacement) : buf_(orig.rdbuf()), str_(orig) {
        orig.rdbuf(replacement.rdbuf());
    }
    ~Stream_Swapper() {
        str_.rdbuf(buf_);
    }
private:
    std::streambuf *buf_;
    std::ostream &str_;
} swapper(std::cerr, taglib_cerr);

/**
 * exceptions yay
 */
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

static void php_exception(const char* msg) {
    zend_class_entry *exception_ce = zend_exception_get_default();
    zend_throw_exception(exception_ce, msg, 0);
}

static bool taglib_error() {
    bool retval = false;
    /**
     * all TagLib errors happen to be prefixed with either
     * "TagLib: " - debug() - tdebug.cpp
     * "*** " - debugData() - tdebug.cpp
     * and std::cerr sometimes contains more than just '\0' 
     */
    if(taglib_cerr.peek() == 'T' || taglib_cerr.peek() == '*') {
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
constexpr unsigned int _charArrForSwitch(const char* str, int index = 0) {
    return !str[index] ? 0x1505 : (_charArrForSwitch(str, index + 1) * 0x21) ^ str[index];
}

constexpr unsigned int operator"" _CASE ( const char str[], size_t size ) {
    return _charArrForSwitch(str);
}

/**
 * I found this on a StackOverflow question about TagLib
 * It works so I'm keeping it for now
 * You can set image tags directly from a file.
 * http://stackoverflow.com/a/8467869
 */
#include <tfile.h>
class ImageFileTest : public TagLib::File {
public:
    ImageFileTest(const char *file) : TagLib::File(file) { }
    TagLib::ByteVector data() { return readBlock(length()); }
private:
    virtual TagLib::Tag *tag() const { return 0; }
    virtual TagLib::AudioProperties *audioProperties() const { return 0; }
    virtual bool save() { return false; }
};

/**
 * making php class constants requires some boilerplate */
#define _defineclassconstant(name, value)           \
    zval * _##name##_ ;                             \
    _##name##_ = (zval *)(pemalloc(sizeof(zval),1));\
    INIT_PZVAL(_##name##_);                         \
    ZVAL_LONG(_##name##_,value);                    \
    zend_hash_add(&ce->constants_table,#name,sizeof(#name),(void *)&_##name##_,sizeof(zval*),NULL);

/**
 * Expose Class Constants from TagLib 
 */
void taglibbase_register_constants(zend_class_entry *ce) {
    /**
     * see TagLib::ID3v2::AttachedPictureFrame::Type in attachedpictureframe.h 
     *
     * For use with get/set ID3v2 functions in this extension.
     */
    _defineclassconstant( APIC_OTHER,              0x00);
    _defineclassconstant( APIC_FILEICON,           0x01);
    _defineclassconstant( APIC_OTHERFILEICON,      0x02);
    _defineclassconstant( APIC_FRONTCOVER,         0x03);
    _defineclassconstant( APIC_BACKCOVER,          0x04);
    _defineclassconstant( APIC_LEAFLETPAGE,        0x05);
    _defineclassconstant( APIC_MEDIA,              0x06);
    _defineclassconstant( APIC_LEADARTIST,         0x07);
    _defineclassconstant( APIC_ARTIST,             0x08);
    _defineclassconstant( APIC_CONDUCTOR,          0x09);
    _defineclassconstant( APIC_BAND,               0x0A);
    _defineclassconstant( APIC_COMPOSER,           0x0B);
    _defineclassconstant( APIC_LYRICIST,           0x0C);
    _defineclassconstant( APIC_RECORDINGLOCATION,  0x0D);
    _defineclassconstant( APIC_DURINGRECORDING,    0x0E);
    _defineclassconstant( APIC_DURINGPERFORMANCE,  0x0F);
    _defineclassconstant( APIC_MOVIESCREENCAPTURE, 0x10);
    _defineclassconstant( APIC_COLOUREDFISH,       0x11);
    _defineclassconstant( APIC_ILLUSTRATION,       0x12);
    _defineclassconstant( APIC_BANDLOGO,           0x13);
    _defineclassconstant( APIC_PUBLISHERLOGO,      0x14);
}

/**
 * public static function getPictureTypeAsString($type)
 */
PHP_METHOD(TagLib, getPictureTypeAsString) {
    int type;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &type) == FAILURE) {
        RETURN_FALSE;
    }

    const char *str;
    switch(type) {
    case 0x00: str = "Other"; break;
    case 0x01: str = "File Icon"; break;
    case 0x02: str = "Other File Icon"; break;
    case 0x03: str = "Front Cover"; break;
    case 0x04: str = "Back Cover"; break;
    case 0x05: str = "Leaflet Page"; break;
    case 0x06: str = "Media"; break;
    case 0x07: str = "Lead Artist"; break;
    case 0x08: str = "Artist"; break;
    case 0x09: str = "Conductor"; break;
    case 0x0A: str = "Band"; break;
    case 0x0B: str = "Composer"; break;
    case 0x0C: str = "Lyricist"; break;
    case 0x0D: str = "Recording Location"; break;
    case 0x0E: str = "During Recording"; break;
    case 0x0F: str = "During Performance"; break;
    case 0x10: str = "Movie Screencapture"; break;
    case 0x11: str = "<°))))><"; break;
    case 0x12: str = "Illustration"; break;
    case 0x13: str = "Band Logo"; break;
    case 0x14: str = "Publisher Logo"; break;
    default:
        RETURN_FALSE;
    }
    RETVAL_STRING((char*)str, 1);
    return;
}
static zend_function_entry php_taglibbase_methods[] = {
    PHP_ME(TagLib, getPictureTypeAsString,  NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    { NULL, NULL, NULL }
};

/**
 * more .h files will be included in each of the .cpp files
 */
#include "TSRM.h"
#include <tlist.h>
#include <tpropertymap.h>
#include <tstringlist.h>

/**
 * just trying to separate out some of this code 
 */
#include "taglibmpeg.cpp"
#include "taglibogg.cpp"
#include "taglibflac.cpp"

zend_class_entry *taglibbase_class_entry;
/**
 * And let's try to unify them into one extension 
 * which provides all of the classes
 */
PHP_MINIT_FUNCTION(taglib_minit) {
    zend_class_entry base_ce;
    zend_class_entry mpeg_ce;
    zend_class_entry ogg_ce;
    zend_class_entry flac_ce;

    INIT_CLASS_ENTRY(base_ce, "TagLib", php_taglibbase_methods);
    taglibbase_class_entry = zend_register_internal_class(&base_ce TSRMLS_CC);
    taglibbase_register_constants(taglibbase_class_entry);

    INIT_CLASS_ENTRY(mpeg_ce, "TagLibMPEG", php_taglibmpeg_methods);
    taglibmpeg_class_entry = zend_register_internal_class(&mpeg_ce TSRMLS_CC);
    //taglibmpeg_register_constants(taglibmpeg_class_entry);

    taglibmpeg_class_entry->create_object = taglibmpegfile_create_handler;
    memcpy(&taglibmpegfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    taglibmpegfile_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ogg_ce, "TagLibOGG", php_taglibogg_methods);
    taglibogg_class_entry = zend_register_internal_class(&ogg_ce TSRMLS_CC);
    taglibogg_register_constants(taglibogg_class_entry);

    taglibogg_class_entry->create_object = tagliboggfile_create_handler;
    memcpy(&tagliboggfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    tagliboggfile_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(flac_ce, "TagLibFLAC", php_taglibflac_methods);
    taglibflac_class_entry = zend_register_internal_class(&flac_ce TSRMLS_CC);
    //taglibflac_register_constants(taglibflac_class_entry);

    taglibflac_class_entry->create_object = taglibflacfile_create_handler;
    memcpy(&taglibflacfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    taglibflacfile_object_handlers.clone_obj = NULL;

    return SUCCESS;
}


static zend_module_dep php_sample_deps[] = {
    ZEND_MOD_REQUIRED("standard")
    {NULL,NULL,NULL}
};

zend_module_entry taglib_module_entry = {

#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER_EX, NULL,
    php_sample_deps,
//    STANDARD_MODULE_HEADER,
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
