#ifndef PHP_TAGLIB_H /* Prevent double inclusion */ 

#define PHP_TAGLIB_H 

/* Define extension properties */ 
#define PHP_TAGLIB_EXTNAME "taglib" 
#define PHP_TAGLIB_EXTVER "1.9.1" 

/* Import configure options
 * when building outside of the
 * PHP source tree */ 
#ifdef HAVE_CONFIG_H 
#include "config.h" 
#endif /* Include PHP standard Header */

#include "php.h" 
/*
 * define the entry point symbol
 * Zend will use when loading this module */ 
extern zend_module_entry taglib_module_entry;
#define phpext_taglib_ptr &taglib_module_entry

ZEND_BEGIN_MODULE_GLOBALS(taglib)
    const char *fileName;
ZEND_END_MODULE_GLOBALS(taglib)

#endif /* PHP_TAGLIB_H */
