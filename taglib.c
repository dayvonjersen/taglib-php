#include "php_taglib.h"
#include <iostream>
#include <iomainip>
#include <stdio.h>
#include <fileref.h>

#include <tag.h>

zend_object_handlers taglib_object_handlers;

struct taglib_object {
    zend_object std;
    Tag *tag;
}

void taglib_free_storage(void *object TSRMLS_DC)
{
    taglib_object *obj = (taglib_object *)object;
    delete obj->tag;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value tag_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    taglib_object *obj = (taglib_object *)emalloc(sizeof(taglib_object));
    memset(obj, 0, sizeof(taglib_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties, (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL, taglib_free_storage, NULL TSRMLS_CC);
    retval.handlers = &taglib_object_handlers;

    return retval;
}

zend_class_entry *tag_ce;

PHP_METHOD(Tag, __construct)
{
    Tag *tag = NULL;
    zval *object = getThis();

    tag = new Tag();
    tag_object *obj = (taglib_object *)zend_object_store_get_object(object TSRMLS_CC);
    obj->tag = tag;
}
PHP_METHOD(Tag, year)
{
    Tag *tag;
    tag_object *obj = (taglib_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    tag = obj->tag;
    if(tag != NULL)
    {
        RETURN_LONG(tag->year());
    } else {
        RETURN_FALSE;
    }
}

static zend_function_entry php_taglib_methods[] = {
    PHP_ME(Tag, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Tag, year,        NULL, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};

PHP_MINIT_FUNCTION(taglibtest)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Tag", php_taglib_methods);
    tag_ce = zend_register_internal_class(&ce TSRMLS_CC);
    tag_ce->create_object = taglib_create_handler;
    memcpy(&taglib_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    taglib_object_handlers.clone_obj = NULL;

    return SUCCESS;
}

zend_module_entry taglib_module_entry = {

#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif

    PHP_TAGLIB_EXTNAME,
    NULL, /* Functions */
    PHP_MINIT(taglibtest), /* MINIT */
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

