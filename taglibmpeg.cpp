#include "TSRM.h"
#include <tlist.h>

#include <mpegfile.h>
#include <mpegheader.h>
#include <mpegproperties.h>
#include <id3v2extendedheader.h>
#include <id3v2footer.h>
#include <id3v2frame.h>
#include <id3v2framefactory.h>
#include <id3v2header.h>
#include <id3v2synchdata.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
#include <commentsframe.h>
#include <generalencapsulatedobjectframe.h>
#include <ownershipframe.h>
#include <popularimeterframe.h>
#include <privateframe.h>
#include <relativevolumeframe.h>
#include <textidentificationframe.h>
#include <uniquefileidentifierframe.h>
#include <unknownframe.h>
#include <unsynchronizedlyricsframe.h>

/**
 * Memory management, ho!" */
zend_object_handlers taglibfile_object_handlers;

struct taglibfile_object {
    zend_object std;
    TagLib::MPEG::File *file;
    TagLib::ID3v2::FrameFactory *frameFactory;
};

void taglibfile_free_storage(void *object TSRMLS_DC)
{
    taglibfile_object *obj = (taglibfile_object *)object;
    delete obj->file;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value taglibfile_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    taglibfile_object *obj = (taglibfile_object *) emalloc(sizeof(taglibfile_object));
    memset(obj, 0, sizeof(taglibfile_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#else
    object_properties_init((zend_object *) &(obj->std.properties), type);
#endif 
    retval.handle = zend_objects_store_put(obj, NULL, taglibfile_free_storage, NULL TSRMLS_CC);
    retval.handlers = &taglibfile_object_handlers;

    return retval;
}

/**
 * end memory management
 * begin class definition */

/**
 * // I'm gonna try to annotate this with pseudo-php where applicable
 * <?php
 *   class TagLibMPEG { ... */
zend_class_entry *taglibmpeg_class_entry;

/**
 *  public function __construct() { ... 
 *  // constructor  */
PHP_METHOD(TagLibMPEG, __construct)
{
    zval *fileName;
    zend_bool readProperties = true;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &fileName, &readProperties) == FAILURE) 
    {
        RETURN_NULL();
    }

    taglibfile_object *thisobj = (taglibfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    thisobj->frameFactory = TagLib::ID3v2::FrameFactory::instance();
    TagLib::MPEG::File *mpegFile = new TagLib::MPEG::File((TagLib::FileName) Z_STRVAL_P(fileName), thisobj->frameFactory, (bool) readProperties);

    if(taglib_error())
    {
        RETURN_NULL();
    }
    thisobj->file = mpegFile;
}
/**
 *  public function getAudioProperties() { ... 
 *  // returns array or false on failure */
PHP_METHOD(TagLibMPEG, getAudioProperties)
{
    taglibfile_object *thisobj = (taglibfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    TagLib::MPEG::Properties *audioProperties = thisobj->file->audioProperties();
    array_init(return_value);
    add_assoc_long(return_value, "length", audioProperties->length());
    add_assoc_long(return_value, "bitrate", audioProperties->bitrate());
    add_assoc_long(return_value, "sampleRate", audioProperties->sampleRate());
    add_assoc_long(return_value, "channels", audioProperties->channels());

    const char *ver = "Unknown";
    switch(audioProperties->version())
    {
        case TagLib::MPEG::Header::Version1:
            ver = "MPEG Version 1";
            break;
        case TagLib::MPEG::Header::Version2:
            ver = "MPEG Version 2";
            break;
        case TagLib::MPEG::Header::Version2_5:
            ver = "MPEG Version 2.5";
            break;
    }
    add_assoc_string(return_value, "version", (char *)ver, 1);

    const char *mode = "Unknown";
    switch(audioProperties->channelMode())
    {
        case TagLib::MPEG::Header::Stereo:
        case TagLib::MPEG::Header::JointStereo:
            mode = "Stereo";
            break;
        case TagLib::MPEG::Header::DualChannel:
            mode = "Dual Mono";
            break;
        case TagLib::MPEG::Header::SingleChannel:
            mode = "Mono";
            break;
    }
    add_assoc_string(return_value, "channelMode", (char *)mode, 1);

    add_assoc_long(return_value, "layer", audioProperties->layer());
    add_assoc_bool(return_value, "protectionEnabled", (zend_bool) audioProperties->protectionEnabled());
    add_assoc_bool(return_value, "isCopyrighted", (zend_bool) audioProperties->isCopyrighted());
    add_assoc_bool(return_value, "isOriginal", (zend_bool) audioProperties->isOriginal());
}

PHP_METHOD(TagLibMPEG, getID3v2)
{
    taglibfile_object *thisobj = (taglibfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if(!thisobj->file->hasID3v2Tag())
    {
        RETURN_FALSE;
    }

    array_init(return_value);

    TagLib::ID3v2::FrameList frameList = thisobj->file->ID3v2Tag()->frameList();
    for(TagLib::List<TagLib::ID3v2::Frame*>::Iterator frame = frameList.begin(); frame != frameList.end(); frame++)
    {
        char *key;
        spprintf(&key, 4, "%s", (*frame)->frameID().data());
        add_assoc_string(return_value, key, (char *) (*frame)->toString().toCString(), 1);
        efree(key);
    }

}

PHP_METHOD(TagLibMPEG, setID3v2)
{
    zval *newFrames;
    taglibfile_object *thisobj = (taglibfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &newFrames) == FAILURE)
    {
        RETURN_FALSE;
    }

    TagLib::ID3v2::Tag *tag = thisobj->file->ID3v2Tag(true);
    TagLib::ID3v2::Header *header = tag->header();
    HashTable *hIndex = Z_ARRVAL_P(newFrames);
    HashPosition pointer;
    zval **data;

    for(zend_hash_internal_pointer_reset_ex(hIndex, &pointer);
        zend_hash_get_current_data_ex(hIndex, (void**)&data, &pointer) == SUCCESS;
        zend_hash_move_forward_ex(hIndex, &pointer))
    {
        char *frameID;
        uint frameID_length, index_type;
        ulong index;
        index_type = zend_hash_get_current_key_ex(hIndex, &frameID, &frameID_length, &index, 0, &pointer);

        if(index_type != HASH_KEY_IS_STRING)
        {
            php_error(E_WARNING, "TagLibMPEG::setID3v2 expects associative array of FRAME_IDs as keys. See http://id3.org/id3v2.3.0#Declared_ID3v2_frames");
            RETURN_FALSE;
            break;
        }
        const TagLib::ByteVector byteVector = TagLib::ByteVector::fromCString(frameID, frameID_length);
        /**
         * this abstraction would make things much, much nicer:
        TagLib::ID3v2::Frame *newFrame = thisobj->frameFactory->createFrame(byteVector);

         * however it seems to cause a segfault if we try to manipulate the frame
         * so let's get stupid */
        switch(_charArrForSwitch(frameID))
        {
            /**
             * AttachedPictureFrame FUN */
            case "APIC"_CASE:
            {
                /**
                 * example argument supplied from php userland:
                 * ["APIC" => ["data" => file_get_contents('album_art.jpg'), // required
                 *             "mime" => "image/jpeg",                       // required
                 *             "type" => TagLibMPEG_AttachedPictureFrame::FrontCover // required TODO: expose constants
                 *             "desc" => "photo by Tavis Lochhead"           // optional
                 *            ]
                 * ]; */
                const char *genericWarning = "AttachedPictureFrame expects a specific array argument, e.g.: ['data' => file_get_contents($newPicture), 'mime' => 'image/jpeg', 'type' => TagLibMPEG_AttachedPictureFrame::FrontCover, 'desc' => 'optional description']";

                TagLib::ID3v2::AttachedPictureFrame *pictureFrame = new TagLib::ID3v2::AttachedPictureFrame(byteVector);
                HashTable *pictureArray = Z_ARRVAL_P(*data);
                zval **data, **mime, **type, **desc;
                if(zend_hash_find(pictureArray, "data", 4, (void **)&data) == SUCCESS)
                {
                    TagLib::ByteVector *dataVector = new TagLib::ByteVector();
                    dataVector->setData(Z_STRVAL_PP(data), Z_STRLEN_PP(desc));
                    pictureFrame->setPicture(*dataVector);
                } else {
                    php_error(E_WARNING, genericWarning);
                    RETURN_FALSE;
                }
                if(zend_hash_find(pictureArray, "mime", 4, (void **)&mime) == SUCCESS)
                {
                    TagLib::String *mimeType = new TagLib::String(Z_STRVAL_PP(mime));
                    pictureFrame->setMimeType(*mimeType);
                } else {
                    php_error(E_WARNING, genericWarning);
                    RETURN_FALSE;
                }
                if(zend_hash_find(pictureArray, "type", 4, (void **)&type) == SUCCESS)
                {
                    using namespace TagLib::ID3v2;
                    AttachedPictureFrame::Type pictureType;
                    switch(Z_LVAL_PP(type))
                    {
                        case AttachedPictureFrame::Other: pictureType = AttachedPictureFrame::Other; break;
                        case AttachedPictureFrame::FileIcon: pictureType = AttachedPictureFrame::FileIcon; break;
                        case AttachedPictureFrame::OtherFileIcon: pictureType = AttachedPictureFrame::OtherFileIcon; break;
                        case AttachedPictureFrame::FrontCover: pictureType = AttachedPictureFrame::FrontCover; break;
                        case AttachedPictureFrame::BackCover: pictureType = AttachedPictureFrame::BackCover; break;
                        case AttachedPictureFrame::LeafletPage: pictureType = AttachedPictureFrame::LeafletPage; break;
                        case AttachedPictureFrame::Media: pictureType = AttachedPictureFrame::Media; break;
                        case AttachedPictureFrame::LeadArtist: pictureType = AttachedPictureFrame::LeadArtist; break;
                        case AttachedPictureFrame::Artist: pictureType = AttachedPictureFrame::Artist; break;
                        case AttachedPictureFrame::Conductor: pictureType = AttachedPictureFrame::Conductor; break;
                        case AttachedPictureFrame::Band: pictureType = AttachedPictureFrame::Band; break;
                        case AttachedPictureFrame::Composer: pictureType = AttachedPictureFrame::Composer; break;
                        case AttachedPictureFrame::Lyricist: pictureType = AttachedPictureFrame::Lyricist; break;
                        case AttachedPictureFrame::RecordingLocation: pictureType = AttachedPictureFrame::RecordingLocation; break;
                        case AttachedPictureFrame::DuringRecording: pictureType = AttachedPictureFrame::DuringRecording; break;
                        case AttachedPictureFrame::DuringPerformance: pictureType = AttachedPictureFrame::DuringPerformance; break;
                        case AttachedPictureFrame::MovieScreenCapture: pictureType = AttachedPictureFrame::MovieScreenCapture; break;
                        case AttachedPictureFrame::ColouredFish: pictureType = AttachedPictureFrame::ColouredFish; break;
                        case AttachedPictureFrame::Illustration: pictureType = AttachedPictureFrame::Illustration; break;
                        case AttachedPictureFrame::BandLogo: pictureType = AttachedPictureFrame::BandLogo; break;
                        case AttachedPictureFrame::PublisherLogo: pictureType = AttachedPictureFrame::PublisherLogo; break;
                        default:
                            php_error(E_WARNING, "Invalid value for AttachedPictureFrame::Type. HINT: Range is 0x00 - 0x14");
                            RETURN_FALSE;
                    }
                    pictureFrame->setType(pictureType);
                } else {
                    php_error(E_WARNING, genericWarning);
                    RETURN_FALSE;
                }
                if(zend_hash_find(pictureArray, "desc", 4, (void **)&desc) == SUCCESS)
                {  
                    TagLib::String *description = new TagLib::String(Z_STRVAL_PP(desc));
                    pictureFrame->setDescription(*description);
                }
                tag->addFrame(pictureFrame);
            }   break;
            /**
             * TextIdentificationFrame FUN */
            case "TALB"_CASE:
            case "TBPM"_CASE:
            case "TCOM"_CASE:
            case "TCON"_CASE:
            case "TCOP"_CASE:
            case "TDAT"_CASE:
            case "TDLY"_CASE:
            case "TENC"_CASE:
            case "TEXT"_CASE:
            case "TFLT"_CASE:
            case "TIME"_CASE:
            case "TIT1"_CASE:
            case "TIT2"_CASE:
            case "TIT3"_CASE:
            case "TKEY"_CASE:
            case "TLAN"_CASE:
            case "TLEN"_CASE:
            case "TMED"_CASE:
            case "TOAL"_CASE:
            case "TOFN"_CASE:
            case "TOLY"_CASE:
            case "TOPE"_CASE:
            case "TORY"_CASE:
            case "TOWN"_CASE:
            case "TPE1"_CASE:
            case "TPE2"_CASE:
            case "TPE3"_CASE:
            case "TPE4"_CASE:
            case "TPOS"_CASE:
            case "TPUB"_CASE:
            case "TRCK"_CASE:
            case "TRDA"_CASE:
            case "TRSN"_CASE:
            case "TRSO"_CASE:
            case "TSIZ"_CASE:
            case "TSRC"_CASE:
            case "TSSE"_CASE:
            case "TYER"_CASE:
            {   TagLib::ID3v2::TextIdentificationFrame *newFrame = new TagLib::ID3v2::TextIdentificationFrame(byteVector);
                TagLib::String *frametext = new TagLib::String(Z_STRVAL_P(*data));

                newFrame->setText(*frametext);
                tag->addFrame(newFrame);
            }   break;
            default:
                php_error(E_WARNING, "Invalid FRAME_ID or not implemented yet.");
        }

        if(taglib_error())
        {
            RETURN_FALSE;
            break;
        }
    }

    if(thisobj->file->save())
        RETURN_TRUE;

    taglib_error();
    RETURN_FALSE;
}


/**
 * Now we assemble the above defined methods into the class or something */
static zend_function_entry php_taglibmpeg_methods[] = {
    PHP_ME(TagLibMPEG, __construct,         NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(TagLibMPEG, getAudioProperties,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, getID3v2,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, setID3v2,  NULL, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
PHP_MINIT_FUNCTION(taglibmpeg_minit)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "TagLibMPEG", php_taglibmpeg_methods);
    taglibmpeg_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
    taglibmpeg_class_entry->create_object = taglibfile_create_handler;
    memcpy(&taglibfile_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    taglibfile_object_handlers.clone_obj = NULL;

    return SUCCESS;
}

zend_module_entry taglib_module_entry = {

#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif

    PHP_TAGLIB_EXTNAME,
    NULL, /* Functions */
    PHP_MINIT(taglibmpeg_minit), /* MINIT */
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
