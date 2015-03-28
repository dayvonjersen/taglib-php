#include <tpropertymap.h>
#include <oggfile.h>
#include <oggflacfile.h>
#include <oggpage.h>
#include <oggpageheader.h>
#include <vorbisfile.h>
#include <vorbisproperties.h>
#include <opusfile.h>
#include <opusproperties.h>
#include <flacfile.h>
#include <flacmetadatablock.h>
#include <flacpicture.h>
#include <flacproperties.h>
#include <xiphcomment.h>

/**
 * TagLib handles oggs based on their codec 
 * so let's make some unnecessary hoops for us to jump through 
 * to abstract it into one TagLibOGG class for PHP rather than three
 */
#define _OGG_VORBIS_ 0x01
#define _OGG_OPUS_   0x02
#define _OGG_FLAC_   0x03
#define _OGG_SPEEX_  0x04
#define _defineclassconstant(name, value) \
zval * _##name##_ ;\
_##name##_ = (zval*)(pemalloc(sizeof(zval),1));\
INIT_PZVAL(_##name##_);\
ZVAL_LONG(_##name##_,value);\
zend_hash_add(&ce->constants_table,#name,sizeof(#name),(void *)&_##name##_,sizeof(zval*),NULL);

struct tagliboggfile_object {
    zend_object std;
    unsigned long type;
    TagLib::Ogg::Vorbis::File *vorbisfile;
    TagLib::Ogg::Opus::File   *opusfile;
    TagLib::Ogg::FLAC::File   *flacfile;
    TagLib::Ogg::XiphComment  *xiphcomment;
};

/*
 * class entry */
zend_class_entry *taglibogg_class_entry;

/**
 * class constants */

void taglibogg_register_constants(zend_class_entry *ce)
{
    _defineclassconstant( VORBIS, _OGG_VORBIS_ );
    _defineclassconstant( OPUS,   _OGG_OPUS_   );
    _defineclassconstant( FLAC,   _OGG_FLAC_   );
    _defineclassconstant( SPEEX,  _OGG_SPEEX_  );
}

/**
 *  public function __construct( $fileName, $type = TagLibOGG::VORBIS ) { ...*/
PHP_METHOD(TagLibOGG, __construct)
{
    zval *fileName;
    long codec = _OGG_VORBIS_;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &fileName, &codec) == FAILURE) 
    {
        RETURN_NULL();
    }

    tagliboggfile_object *thisobj = (tagliboggfile_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    TagLib::FileName oggFileName = (TagLib::FileName) Z_STRVAL_P(fileName);
    switch(codec)
    {
        case _OGG_VORBIS_:
            thisobj->type        = _OGG_VORBIS_;
            thisobj->vorbisfile  = new TagLib::Ogg::Vorbis::File(oggFileName);
            thisobj->xiphcomment = thisobj->vorbisfile->tag();
            break;
        case _OGG_OPUS_:
            thisobj->type        = _OGG_OPUS_;
            thisobj->opusfile    = new TagLib::Ogg::Opus::File(oggFileName);
            thisobj->xiphcomment = thisobj->opusfile->tag();
            break;
        case _OGG_FLAC_:
            thisobj->type        = _OGG_FLAC_;
            thisobj->flacfile    = new TagLib::Ogg::FLAC::File(oggFileName);
            thisobj->xiphcomment = thisobj->flacfile->tag();
            break;
        case _OGG_SPEEX_:
            php_error(E_DEPRECATED, "Speex is deprecated.");
        default:
            php_error(E_WARNING, "Unrecognized or unsupported option for $codec in TagLibOGG::__construct()");
            RETURN_NULL();
    }

    if(taglib_error())
    {
        RETURN_NULL();
    }
}
/**
 *  public function getAudioProperties() { */
PHP_METHOD(TagLibOGG, getAudioProperties)
{
    tagliboggfile_object *thisobj = (tagliboggfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    array_init(return_value);
    switch(thisobj->type)
    {
        case _OGG_VORBIS_:
        {
            TagLib::Vorbis::Properties *audioProperties = thisobj->vorbisfile->audioProperties();

            add_assoc_long(return_value, "length", audioProperties->length());
            add_assoc_long(return_value, "bitrate", audioProperties->bitrate());
            add_assoc_long(return_value, "sampleRate", audioProperties->sampleRate());
            add_assoc_long(return_value, "channels", audioProperties->channels());
            add_assoc_long(return_value, "vorbisVersion", audioProperties->vorbisVersion());
            add_assoc_long(return_value, "bitrateMaximum", audioProperties->bitrateMaximum());
            add_assoc_long(return_value, "bitrateNominal", audioProperties->bitrateNominal());
            add_assoc_long(return_value, "bitrateMinimum", audioProperties->bitrateMinimum());
        } break;
        case _OGG_OPUS_:
        {
            TagLib::Ogg::Opus::Properties *audioProperties = thisobj->opusfile->audioProperties();

            add_assoc_long(return_value, "length", audioProperties->length());
            add_assoc_long(return_value, "bitrate", audioProperties->bitrate());
            add_assoc_long(return_value, "sampleRate", audioProperties->sampleRate());
            add_assoc_long(return_value, "channels", audioProperties->channels());
            add_assoc_long(return_value, "opusVersion", audioProperties->opusVersion());
            add_assoc_long(return_value, "inputSampleRate", audioProperties->inputSampleRate());
        } break;
        case _OGG_FLAC_:
        {
            TagLib::Ogg::FLAC::Properties *audioProperties = thisobj->flacfile->audioProperties();

            add_assoc_long(return_value, "length", audioProperties->length());
            add_assoc_long(return_value, "bitrate", audioProperties->bitrate());
            add_assoc_long(return_value, "sampleRate", audioProperties->sampleRate());
            add_assoc_long(return_value, "channels", audioProperties->channels());
            add_assoc_long(return_value, "sampleWidth", audioProperties->sampleWidth());
            add_assoc_long(return_value, "sampleFrames", audioProperties->sampleFrames());
            add_assoc_string(return_value, "signature", audioProperties->signature().data(), 1);
        } break;
    }
}

PHP_METHOD(TagLibOGG, getXiphComment)
{
    tagliboggfile_object *thisobj = (tagliboggfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    array_init(return_value);

    TagLib::PropertyMap propMap = thisobj->xiphcomment->properties();

    for(TagLib::Map<TagLib::String,TagLib::StringList>::Iterator property = propMap.begin(); property != propMap.end(); property++)
    {
        add_assoc_string(return_value, property->first.toCString(), (char *)(property->second.toString().toCString()), 1);
    }
}

/**
 * returns FALSE if something went wrong,
 * returns TRUE if everything went through */
PHP_METHOD(TagLibOGG, setXiphComment)
{
    zval *newProperties;
    zend_bool overwrite_existing_tags = false;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &newProperties, &overwrite_existing_tags) == FAILURE)
    {
        RETURN_FALSE;
    }
    
    tagliboggfile_object *thisobj = (tagliboggfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    HashTable *hIndex = Z_ARRVAL_P(newProperties);
    HashPosition pointer;
    zval **data;
    for(zend_hash_internal_pointer_reset_ex(hIndex, &pointer);
        zend_hash_get_current_data_ex(hIndex, (void**)&data, &pointer) == SUCCESS;
        zend_hash_move_forward_ex(hIndex, &pointer))
    {
        char *key;
        uint key_length, key_type;
        ulong index;
        key_type = zend_hash_get_current_key_ex(hIndex, &key, &key_length, &index, 0, &pointer);

        if(key_type != HASH_KEY_IS_STRING)
        {
            php_error(E_WARNING, "TagLibOGG::setXiphComment expects associative array!");
            RETURN_FALSE;
            break;
        }

        TagLib::String *destKey   = new TagLib::String((const char *)key);
        TagLib::String *destValue = new TagLib::String(Z_STRVAL_PP(data));

        thisobj->xiphcomment->addField(*destKey,*destValue,(bool)overwrite_existing_tags);
        if(taglib_error())
        {
            php_error(E_WARNING, "XiphComment::addField() failed.");
            break;
        }
    }

    switch(thisobj->type)
    {
        case _OGG_VORBIS_:
            if(thisobj->vorbisfile->save())
                RETURN_TRUE;
            break;
        case _OGG_OPUS_:
            if(thisobj->opusfile->save())
                RETURN_TRUE;
            break;
        case _OGG_FLAC_:
            if(thisobj->flacfile->save())
                RETURN_TRUE;
            break;
    }
    taglib_error();
    RETURN_FALSE;
}

/**
 * Now we assemble the above defined methods into the class or something */
static zend_function_entry php_taglibogg_methods[] = {
    PHP_ME(TagLibOGG, __construct,         NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(TagLibOGG, getAudioProperties,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibOGG, getXiphComment,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibOGG, setXiphComment,      NULL, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
PHP_MINIT_FUNCTION(taglibogg_minit)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "TagLibOGG", php_taglibogg_methods);
    taglibogg_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
    taglibogg_register_constants(taglibogg_class_entry);

//    taglibogg_class_entry->create_object = taglibfile_create_handler;
//    memcpy(&taglibfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
//    taglibfile_object_handlers.clone_obj = NULL;


    return SUCCESS;
}

zend_module_entry taglib_module_entry = {

#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif

    PHP_TAGLIB_EXTNAME,
    NULL, /* Functions */
    PHP_MINIT(taglibogg_minit), /* MINIT */
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
