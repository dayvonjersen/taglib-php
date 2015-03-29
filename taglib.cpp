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
 * using extensions within extensions 
 * XXX don't know what to link against so no.
 *#include <ext/standard/base64.h>*/

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
 * XXX I can't figure out how to properly link at compile time in order to get
 * XXX php core functions, namely php_base64_encode() and php_base64_decode()
 * XXX to work. #include <ext/standard/base64.h> will let it compile but
 * XXX when it is called at runtime, it produces an undefined symbol error
 * XXX If you have any insight into this matter, please email me: tso@teknik.io
 */
/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

typedef enum
{
	step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
	base64_encodestep step;
	char result;
	int stepcount;
} base64_encodestate;

const int CHARS_PER_LINE = 72;

void base64_init_encodestate(base64_encodestate* state_in)
{
	state_in->step = step_A;
	state_in->result = 0;
	state_in->stepcount = 0;
}

char base64_encode_value(char value_in)
{
	static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (value_in > 63) return '=';
	return encoding[(int)value_in];
}

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in)
{
	const char* plainchar = plaintext_in;
	const char* const plaintextend = plaintext_in + length_in;
	char* codechar = code_out;
	char result;
	char fragment;
	
	result = state_in->result;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_A:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_A;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result = (fragment & 0x0fc) >> 2;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x003) << 4;
	case step_B:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_B;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0f0) >> 4;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x00f) << 2;
	case step_C:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_C;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0c0) >> 6;
			*codechar++ = base64_encode_value(result);
			result  = (fragment & 0x03f) >> 0;
			*codechar++ = base64_encode_value(result);
			
			++(state_in->stepcount);
			if (state_in->stepcount == CHARS_PER_LINE/4)
			{
				*codechar++ = '\n';
				state_in->stepcount = 0;
			}
		}
	}
	/* control should not reach here */
	return codechar - code_out;
}

int base64_encode_blockend(char* code_out, base64_encodestate* state_in)
{
	char* codechar = code_out;
	
	switch (state_in->step)
	{
	case step_B:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		*codechar++ = '=';
		break;
	case step_C:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		break;
	case step_A:
		break;
	}
	*codechar++ = '\n';
	
	return codechar - code_out;
}
/*
cdecoder.c - c source to a base64 decoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/
typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

int base64_decode_value(char value_in)
{
	static const char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
	static const char decoding_size = sizeof(decoding);
	value_in -= 43;
	if (value_in < 0 || value_in > decoding_size) return -1;
	return decoding[(int)value_in];
}

void base64_init_decodestate(base64_decodestate* state_in)
{
	state_in->step = step_a;
	state_in->plainchar = 0;
}

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
{
	const char* codechar = code_in;
	char* plainchar = plaintext_out;
	char fragment;
	
	*plainchar = state_in->plainchar;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_a:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_a;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar    = (fragment & 0x03f) << 2;
	case step_b:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_b;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x030) >> 4;
			*plainchar    = (fragment & 0x00f) << 4;
	case step_c:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_c;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x03c) >> 2;
			*plainchar    = (fragment & 0x003) << 6;
	case step_d:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_d;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++   |= (fragment & 0x03f);
		}
	}
	/* control should not reach here */
	return plainchar - plaintext_out;
}

/**
 * XXX end awful workaround */

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
