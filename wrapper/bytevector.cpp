/**
 * taglib header file */
#include <tbytevector.h>

/**
 * magic incantations */
zend_class_entry *php_taglib_bytevector;
#define PHP_TAGLIB_BYTEVECTOR "ByteVector"

/**
 * namespace for this class */
using namespace TagLib::ByteVector;

/**
 * methods -- mapping cpp to zend */
PHP_METHOD(ByteVector, __construct)
{
    /**
     * all possible arguments */
    uint size;
    char value;
    const ByteVector *v;
    uint offset;
    uint length;
    char c;
    char *data;
    uint length;

    ByteVector returnObj;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "0") == SUCCESS) {
        returnObj = new ByteVector();        
    } else if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "1", &v) == SUCCESS) {
        returnObj = new ByteVector(&v);
    } else if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "1", &c) == SUCCESS) {
        returnObj = new ByteVector(c);
    } else if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "1", &data) == SUCCESS) {
        returnObj = new ByteVector(&data);
    } else if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "2", &data, &length) == SUCCESS) {
        returnObj = new ByteVector(&data,length);
    } else {
        return;
    }

    RETURN_OBJECT(returnObj);
}

/**
 * methods -- arguments */
static
    ZEND_BEGIN_ARG_INFO_EX(php_taglib_one_arg, 0, 0, 1)
    ZEND_END_ARG_INFO()
static
    ZEND_BEGIN_ARG_INFO_EX(php_taglib_two_args, 0, 0, 2)
    ZEND_END_ARG_INFO()

/**
 * methods -- listing all */
static zend_function_entry php_taglib_bytevector_methods[] = {
    PHP_ME(ByteVector, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)

};
