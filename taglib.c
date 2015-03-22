#include "php_taglib.h"

/**
 * test function to see if this works */
PHP_FUNCTION(taglib_test_func)
{
    char *name;
    int name_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE)
    {
        RETURN_NULL();
    }

    php_printf("Hello, ");
    PHPWRITE(name, name_len);
    php_printf("!\n");
}

static zend_function_entry php_taglib_functions[] = {
    PHP_FE(taglib_test_func, NULL) 
    { NULL, NULL, NULL }
};

zend_module_entry taglib_module_entry = {

#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif

    PHP_TAGLIB_EXTNAME,
    php_taglib_functions, /* Functions */
    NULL, /* MINIT */
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

