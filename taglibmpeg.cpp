/**
 * all aboard 
 */
#include <mpegfile.h>
#include <mpegheader.h>
#include <mpegproperties.h>
#include <apefile.h>
#include <apefooter.h>
#include <apeitem.h>
#include <apeproperties.h>
#include <apetag.h>
#include <id3v1tag.h>
#include <id3v1genres.h>
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
#include <urllinkframe.h>

/**
 * Memory management, ho!" 
 */
zend_object_handlers taglibmpegfile_object_handlers;

struct taglibmpegfile_object {
    zend_object std;
    TagLib::MPEG::File *file;
    TagLib::ID3v2::FrameFactory *frameFactory;
};

void taglibmpegfile_free_storage(void *object TSRMLS_DC) {
    taglibmpegfile_object *obj = (taglibmpegfile_object *)object;
    delete obj->file;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value taglibmpegfile_create_handler(zend_class_entry *type TSRMLS_DC) {
    zval *tmp;
    zend_object_value retval;

    taglibmpegfile_object *obj = (taglibmpegfile_object *) emalloc(sizeof(taglibmpegfile_object));
    memset(obj, 0, sizeof(taglibmpegfile_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#else
    object_properties_init((zend_object *) &(obj->std.properties), type);
#endif 
    retval.handle = zend_objects_store_put(obj, NULL, taglibmpegfile_free_storage, NULL TSRMLS_CC);
    retval.handlers = &taglibmpegfile_object_handlers;

    return retval;
}

/**
 * end memory management
 * begin class definition 
 */
zend_class_entry *taglibmpeg_class_entry;

/**
 *  public function __construct() {
 *
 *  throws Exception on failure because that's the only way to ensure a new
 *  object doesn't get created in that case.
 */
PHP_METHOD(TagLibMPEG, __construct) {
    zval *fileName;
    zend_bool readProperties = true;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &fileName, &readProperties) == FAILURE) {
    php_exception("Expected filename in constructor");
        RETURN_FALSE;
    } 

    if(Z_TYPE_P(fileName) != IS_STRING) {
    php_exception("Expected filename in constructor to be a string.");
        RETURN_FALSE;
    }

    const char* filestr = Z_STRVAL_P(fileName);

    if(!TagLib::MPEG::File::isReadable(filestr)) {
        char msg[sizeof(filestr)+25];
        php_sprintf(msg, "%s cannot be open or read", filestr);
        php_exception((const char*)msg);
        RETURN_FALSE;
    }

    if(!TagLib::MPEG::File::isWritable(filestr)) {
        char msg[sizeof(filestr)+22];
        php_sprintf(msg, "%s cannot be written to", filestr);
        php_exception((const char*)msg);
        RETURN_FALSE;
    }

    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    thisobj->frameFactory = TagLib::ID3v2::FrameFactory::instance();
    try {
        thisobj->file = new TagLib::MPEG::File((TagLib::FileName) filestr, thisobj->frameFactory, (bool) readProperties);
    } catch(std::exception& e) {
        php_error(E_WARNING, "%s", e.what());
        php_exception(e.what());
        RETURN_FALSE;
    }

    if(!thisobj->file->isValid()) {
        char msg[sizeof(filestr)+24];
        php_sprintf(msg, "%s cannot be open or read", filestr);
        php_exception((const char*)msg);
        RETURN_FALSE;
    }

    if(taglib_error()) {
        delete thisobj->file;
        php_exception("taglib returned error");
        RETURN_FALSE;
    }
}

/**
 *  public function getAudioProperties()
 *
 *  returns array or false on failure
 */
PHP_METHOD(TagLibMPEG, getAudioProperties) {
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    TagLib::MPEG::Properties *audioProperties = thisobj->file->audioProperties();
    array_init(return_value);
    add_assoc_long(return_value, "length", audioProperties->length());
    add_assoc_long(return_value, "bitrate", audioProperties->bitrate());
    add_assoc_long(return_value, "sampleRate", audioProperties->sampleRate());
    add_assoc_long(return_value, "channels", audioProperties->channels());

    const char *ver = "Unknown";
    switch(audioProperties->version()) {
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
    switch(audioProperties->channelMode()) {
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

    add_assoc_long(return_value, "layer",             audioProperties->layer());
    add_assoc_bool(return_value, "protectionEnabled", (zend_bool) audioProperties->protectionEnabled());
    add_assoc_bool(return_value, "isCopyrighted",     (zend_bool) audioProperties->isCopyrighted());
    add_assoc_bool(return_value, "isOriginal",        (zend_bool) audioProperties->isOriginal());
}

/**
 * public function hasID3v1() 
 */
PHP_METHOD(TagLibMPEG, hasID3v1) {
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    RETVAL_BOOL(thisobj->file->hasID3v1Tag());
}

/**
 * public function hasID3v2()
 */
PHP_METHOD(TagLibMPEG, hasID3v2) {
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    RETVAL_BOOL(thisobj->file->hasID3v2Tag());
}

/**
 * public function getID3v1()
 */
PHP_METHOD(TagLibMPEG, getID3v1) {
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if(!thisobj->file->hasID3v1Tag()) {
        RETURN_FALSE;
    }

    TagLib::ID3v1::Tag *id3v1 = thisobj->file->ID3v1Tag();
    array_init(return_value);

    TagLib::PropertyMap propMap = id3v1->properties();

    for(TagLib::Map<TagLib::String,TagLib::StringList>::Iterator property = propMap.begin(); property != propMap.end(); property++) {
        add_assoc_string(return_value, property->first.toCString(), (char *)(property->second.toString().toCString()), 1);
    }
}

/**
 * public function setID3v1()
 *
 * returns TRUE if everything went through
 * returns FALSE if something went wrong
 * returns array of tags which failed to be set
 */
PHP_METHOD(TagLibMPEG, setID3v1) {
    zval *newProperties;
    zend_bool overwrite_existing_tags = false;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &newProperties, &overwrite_existing_tags) == FAILURE) {
        RETURN_FALSE;
    }
    if(Z_TYPE_P(newProperties) != IS_ARRAY) {
        RETURN_FALSE;
    }
    
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    TagLib::ID3v1::Tag *id3v1;

    if(!thisobj->file->hasID3v1Tag()) {
    id3v1 = thisobj->file->ID3v1Tag(true);
    overwrite_existing_tags = true;

        TagLib::PropertyMap propMap = id3v1->properties();
    propMap.removeEmpty();
    propMap.erase(propMap);
    } else {
    id3v1 = thisobj->file->ID3v1Tag();
    }

    TagLib::PropertyMap propMap = id3v1->properties();

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

        if(key_type != HASH_KEY_IS_STRING) {
            php_error(E_WARNING, "TagLibMPEG::setID3v1 expects associative array of string values!");
            RETURN_FALSE;
            break;
        }

        if(Z_TYPE_PP(data) != IS_STRING) {
            php_error(E_WARNING, "TagLibMPEG::setID3v1 expects associative array of string values!");
            RETURN_FALSE;
            break;
        }
        TagLib::String *destKey = new TagLib::String((const char *)key);
        TagLib::StringList *destValue = new TagLib::StringList(*(new TagLib::String(Z_STRVAL_PP(data))));

        // default PropertyMap::insert() behavior is append
        if(propMap.contains(*destKey) && overwrite_existing_tags) {
            propMap.erase(*destKey);
        }

        if(!propMap.insert(*destKey,*destValue) || taglib_error()) {
            php_error(E_WARNING, "PropertyMap::insert() failed, possibly invalid key provided.");
            break;
        }
    }

    TagLib::PropertyMap failedToSet = id3v1->setProperties(propMap);
    if(thisobj->file->save(TagLib::MPEG::File::TagTypes::ID3v1)) {
        if(failedToSet.begin() == failedToSet.end()) {
            RETURN_TRUE;
        } else {
            array_init(return_value);
            for(TagLib::Map<TagLib::String,TagLib::StringList>::Iterator property = failedToSet.begin(); 
                property != failedToSet.end(); 
                property++) {
                add_assoc_string(return_value, property->first.toCString(), (char *)(property->second.toString().toCString()), 1);
            }       
        } 
    } else {
        taglib_error();
        RETURN_FALSE;   
    }
}

/**
 * public function getID3v2()
 */
PHP_METHOD(TagLibMPEG, getID3v2) {
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if(!thisobj->file->hasID3v2Tag()) {
        RETURN_FALSE;
    }

    TagLib::ID3v2::Tag *tag = thisobj->file->ID3v2Tag(true);

    array_init(return_value);

    TagLib::ID3v2::FrameList frameList = thisobj->file->ID3v2Tag()->frameList();
    for(TagLib::List<TagLib::ID3v2::Frame*>::Iterator frame = frameList.begin(); frame != frameList.end(); frame++) {
        char *key;
        spprintf(&key, 4, "%s", (*frame)->frameID().data());

        zval *subarray;
        MAKE_STD_ZVAL(subarray);
        array_init(subarray);

        add_assoc_string(subarray,"frameID",key,1);

        switch(_charArrForSwitch(key)) {
        case "APIC"_CASE:
        {   
            TagLib::ID3v2::AttachedPictureFrame *apic = (TagLib::ID3v2::AttachedPictureFrame*)(*frame);
            int retLen;
            unsigned char *picdat = php_base64_encode((const unsigned char *)apic->picture().data(), apic->picture().size(), &retLen); 

            add_assoc_string(subarray, "data", (char*)picdat, 1);
            add_assoc_string(subarray, "mime", (char*)(apic->mimeType().toCString()),1);
            add_assoc_long(  subarray, "type", apic->type());
            add_assoc_string(subarray, "desc", (char*)(apic->description().toCString()),1);
        }   break;
        default:
            add_assoc_string(subarray, "data", (char *) (*frame)->toString().toCString(), 1);
        }

        add_next_index_zval(return_value, subarray);
    }
}

/**
 * public function stripTags()
 *
 * returns TRUE on success, FALSE + E_WARNING on failure
 */
PHP_METHOD(TagLibMPEG, stripTags) {
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    /**
     * NOTE: calling MPEG::File::strip() with other arguments failed to remove any tags from file
     */
    if(thisobj->file->strip(0x0002)) {
        if(thisobj->file->save(0x0000, true, 3)) {
            RETURN_TRUE;
        } else {
            php_error(E_WARNING, "Failed to save changes to file!");
        }
    } else {
        php_error(E_WARNING, "Failed to strip tags from file!");
    }
    RETURN_FALSE;
}

/**
 * pulling this out into a separate function for reuse with Taglib::FLAC
 */
static bool id3v2_set_frame(TagLib::ID3v2::Tag *tag, zval **data, TagLib::ByteVector byteVector, char *frameID) {
   /**
     * this abstraction would make things much, much nicer:
     * TagLib::ID3v2::Frame *newFrame = thisobj->frameFactory->createFrame(byteVector);
     *
     * however it seems to cause a segfault if we try to manipulate the frame
     * so let's get stupid 
     */
    switch(_charArrForSwitch(frameID)) {

        /**
         * AttachedPictureFrame
         */
        case "APIC"_CASE:
        {
            const char *genericWarning = "AttachedPictureFrame expects a specific array argument, e.g.: \
    [\
     'data' => base64_encode(file_get_contents($newPicture)),\
     'mime' => 'image/jpeg',\
     'type' => TagLib::APIC_FRONTCOVER,\
     'desc' => 'optional description'\
    ]";
            TagLib::ID3v2::AttachedPictureFrame *pictureFrame = new TagLib::ID3v2::AttachedPictureFrame(byteVector);
            if(Z_TYPE_PP(data) != IS_ARRAY) {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            HashTable *pictureArray = Z_ARRVAL_P(*data);
            zval **apic_data, **apic_file, **apic_mime, **apic_type, **apic_desc;
            if(zend_hash_find(pictureArray, "data", 5, (void **)&apic_data) == SUCCESS) {
                if(Z_TYPE_PP(apic_data) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                int decsize;
                unsigned char *b64data = php_base64_decode((const unsigned char *)Z_STRVAL_PP(apic_data),Z_STRLEN_PP(apic_data),&decsize);
                TagLib::ByteVector dataVector = TagLib::ByteVector::fromCString((const char*)b64data,(size_t)decsize);
                pictureFrame->setPicture(dataVector); 
            } else if(zend_hash_find(pictureArray, "file", 5, (void **)&apic_file) == SUCCESS) {
                if(Z_TYPE_PP(apic_file) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                ImageFileTest *image = new ImageFileTest(Z_STRVAL_PP(apic_file));
                pictureFrame->setPicture(image->readBlock(image->length()));
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            if(zend_hash_find(pictureArray, "mime", 5, (void **)&apic_mime) == SUCCESS) {
                if(Z_TYPE_PP(apic_mime) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *mimeType = new TagLib::String(Z_STRVAL_PP(apic_mime));
                pictureFrame->setMimeType(*mimeType);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            if(zend_hash_find(pictureArray, "type", 5, (void **)&apic_type) == SUCCESS) {
                if(Z_TYPE_PP(apic_type) != IS_LONG) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                int type = Z_LVAL_PP(apic_type);
                if(type >= 0x0 && type <= 0x14) {
                    pictureFrame->setType((TagLib::ID3v2::AttachedPictureFrame::Type)type);
                } else {
                    php_error(E_WARNING, "Invalid value for AttachedPictureFrame::Type. Try using TagLib::APIC_* constants");
                    return false;
                }
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            if(zend_hash_find(pictureArray, "desc", 5, (void **)&apic_desc) == SUCCESS) {
                if(Z_TYPE_PP(apic_desc) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *description = new TagLib::String(Z_STRVAL_PP(apic_desc));
                pictureFrame->setDescription(*description);
            }
            tag->addFrame(pictureFrame);
        }   break;

        /**
         * TextIdentificationFrame
         */
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
        case "TMOO"_CASE:
        case "TSOP"_CASE:
        case "TSOT"_CASE:
        case "TSOA"_CASE:
        case "TDRC"_CASE:
        {
            if(Z_TYPE_PP(data) != IS_STRING) {
                php_error(E_WARNING, "Expected string value in setting %s", frameID);
                return false;
            }

            TagLib::ID3v2::TextIdentificationFrame *newFrame = new TagLib::ID3v2::TextIdentificationFrame(byteVector);
            TagLib::String *frametext = new TagLib::String(Z_STRVAL_P(*data));

            newFrame->setText(*frametext);
            tag->addFrame(newFrame);
        }   break;

        /**
         * UserTextIdentificationFrame
         */
        case "TXXX"_CASE:
        {   
            TagLib::ID3v2::UserTextIdentificationFrame *newFrame = new TagLib::ID3v2::UserTextIdentificationFrame(byteVector);

            const char* genericWarning = "TXXX aka UserTextIdentificationFrame requires an array argument e.g.\
    [\
    'desc' => 'Description of Frame',\
    'text' => ''\
    ]";

            if(Z_TYPE_PP(data) != IS_ARRAY) {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            HashTable *txxxArray = Z_ARRVAL_PP(data);
            zval **desc, **text;
            if(zend_hash_find(txxxArray, "desc", 5, (void **)&desc) == SUCCESS) {
                if(Z_TYPE_PP(desc) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *framedesc = new TagLib::String(Z_STRVAL_PP(desc));
                newFrame->setDescription(*framedesc);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }   
            if(zend_hash_find(txxxArray, "text", 5, (void **)&text) == SUCCESS) {
                if(Z_TYPE_PP(text) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *frametext = new TagLib::String(Z_STRVAL_PP(text));
                newFrame->setText(*frametext);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            tag->addFrame(newFrame);
        }   break;

        /**
         * CommentsFrame
         */
        case "COMM"_CASE:
        {
            if(Z_TYPE_PP(data) != IS_STRING) {
                php_error(E_WARNING, "Expected string for %s", frameID);
                return false;
            }
            TagLib::ID3v2::CommentsFrame *newFrame = new TagLib::ID3v2::CommentsFrame(byteVector);
            TagLib::String *frametext = new TagLib::String(Z_STRVAL_P(*data));

            newFrame->setText(*frametext);
            tag->addFrame(newFrame);
        }   break;

        /**
         * UrlLinkFrame
         */
        case "WCOM"_CASE:
        case "WCOP"_CASE:
        case "WOAF"_CASE:
        case "WOAR"_CASE:
        case "WOAS"_CASE:
        case "WORS"_CASE:
        case "WPAY"_CASE:
        case "WPUB"_CASE:
        {
            if(Z_TYPE_PP(data) != IS_STRING) {
                php_error(E_WARNING, "Expected string for %s", frameID);
                return false;
            }
            
            TagLib::ID3v2::UrlLinkFrame *newFrame = new TagLib::ID3v2::UrlLinkFrame(byteVector);
            TagLib::String *frameURL = new TagLib::String(Z_STRVAL_P(*data));
            newFrame->setUrl(*frameURL);
            tag->addFrame(newFrame);
        } break;

        /**
         * UserUrlLinkFrame
         */
        case "WXXX"_CASE:
        {   
            TagLib::ID3v2::UserUrlLinkFrame *newFrame = new TagLib::ID3v2::UserUrlLinkFrame(byteVector);
            const char* genericWarning = "WXXX aka UserUrlLinkFrame requires an array argument e.g. \
    [\
        'desc' => 'Description of URL',\
        'text' => 'http://www.example.com/'\
    ]";

            if(Z_TYPE_PP(data) != IS_ARRAY) {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            HashTable *txxxArray = Z_ARRVAL_PP(data);
            zval **desc, **text;
            if(zend_hash_find(txxxArray, "desc", 5, (void **)&desc) == SUCCESS) {
                if(Z_TYPE_PP(desc) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *framedesc = new TagLib::String(Z_STRVAL_PP(desc));
                newFrame->setDescription(*framedesc);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }   
            if(zend_hash_find(txxxArray, "text", 5, (void **)&text) == SUCCESS) {
                if(Z_TYPE_PP(text) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *frametext = new TagLib::String(Z_STRVAL_PP(text));
                newFrame->setUrl(*frametext);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            tag->addFrame(newFrame);
        } break;

        /**
         * OwnershipFrame 
         */
        case "OWNE"_CASE:
        {
            TagLib::ID3v2::OwnershipFrame *newFrame = new TagLib::ID3v2::OwnershipFrame(byteVector);
            const char* genericWarning = "OWNE aka OwnershipFrame requires an array argument e.g. \
    [\
        'date' => '19691231',\
        'paid' => '$0.99',\
        'seller' => 'someone'\
    ]";

            if(Z_TYPE_PP(data) != IS_ARRAY) {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            HashTable *txxxArray = Z_ARRVAL_PP(data);
            zval **date, **paid, **seller;
            if(zend_hash_find(txxxArray, "date", 5, (void **)&date) == SUCCESS) {
                if(Z_TYPE_PP(date) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *datepurchased = new TagLib::String(Z_STRVAL_PP(date));
                newFrame->setDatePurchased(*datepurchased);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            if(zend_hash_find(txxxArray, "paid", 5, (void **)&paid) == SUCCESS) {
                if(Z_TYPE_PP(paid) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *pricepaid = new TagLib::String(Z_STRVAL_PP(paid));
                newFrame->setPricePaid(*pricepaid);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            if(zend_hash_find(txxxArray, "seller", 7, (void **)&seller) == SUCCESS) {
                if(Z_TYPE_PP(seller) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *sellout = new TagLib::String(Z_STRVAL_PP(seller));
                newFrame->setSeller(*sellout);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            tag->addFrame(newFrame);
        } break;

        /**
         * PrivateFrame 
         */
        case "PRIV"_CASE:
        {
            TagLib::ID3v2::PrivateFrame *newFrame = new TagLib::ID3v2::PrivateFrame();
            const char* genericWarning = "PRIV aka PrivateFrame requires an array argument e.g. \
    [\
        'owner' => 'nobody@example.com',\
        'data' => base64_encode('some data...')\
    ]";
            
            if(Z_TYPE_PP(data) != IS_ARRAY) {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            HashTable *txxxArray = Z_ARRVAL_PP(data);
            zval **privowner, **privdata;
            if(zend_hash_find(txxxArray, "owner", 6, (void **)&privowner) == SUCCESS) {
                if(Z_TYPE_PP(privowner) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *owner = new TagLib::String(Z_STRVAL_PP(privowner));
                newFrame->setOwner(*owner);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }

            if(zend_hash_find(txxxArray, "data", 5, (void **)&privdata) == SUCCESS) {
                if(Z_TYPE_PP(privdata) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                int decsize;
                unsigned char *b64data = php_base64_decode((const unsigned char*)Z_STRVAL_PP(privdata),Z_STRLEN_PP(privdata),&decsize);
                TagLib::ByteVector dataVector = TagLib::ByteVector::fromCString((const char*)b64data,(size_t) decsize);
                newFrame->setData(dataVector);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            tag->addFrame(newFrame);
        } break;

        /**
         * UniqueFileIdentifierFrame 
         */
        case "UFID"_CASE:
        {
            TagLib::ID3v2::UniqueFileIdentifierFrame *newFrame = new TagLib::ID3v2::UniqueFileIdentifierFrame(byteVector);
            const char* genericWarning = "UFID aka UniqueFileIdentifierFrame requires an array argument e.g. \
    [\
        'owner' => 'http://somemusicdatabase.example.com/',\
        'id' => '123456789'\
    ]";

            if(Z_TYPE_PP(data) != IS_ARRAY) {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            HashTable *txxxArray = Z_ARRVAL_PP(data);
            zval **owner, **id;
            if(zend_hash_find(txxxArray, "owner", 6, (void **)&owner) == SUCCESS) {
                if(Z_TYPE_PP(owner) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::String *ufidowner = new TagLib::String(Z_STRVAL_PP(owner));
                newFrame->setOwner(*ufidowner);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            if(zend_hash_find(txxxArray, "id", 3, (void **)&id) == SUCCESS) {
                if(Z_TYPE_PP(id) != IS_STRING) {
                    php_error(E_WARNING, genericWarning);
                    return false;
                }
                TagLib::ByteVector whyIsThisAByteVector = TagLib::ByteVector::fromCString((const char*) Z_STRVAL_PP(id));
                newFrame->setIdentifier(whyIsThisAByteVector);
            } else {
                php_error(E_WARNING, genericWarning);
                return false;
            }
            tag->addFrame(newFrame);
        } break;

        /**
         * UnsynchronizedLyricsFrame */
        case "USLT"_CASE:
        {
            if(Z_TYPE_PP(data) != IS_STRING) {
                php_error(E_WARNING, "Expected string for %s", frameID);
                return false;
            }
            
            TagLib::ID3v2::UnsynchronizedLyricsFrame *newFrame = new TagLib::ID3v2::UnsynchronizedLyricsFrame(byteVector);
            /**
             * language and description typically get ignored apparently 
             */
            TagLib::String *lyrics = new TagLib::String(Z_STRVAL_P(*data));
            newFrame->setText(*lyrics);
            tag->addFrame(newFrame);
        } break;

        /**
         * ... */
        case "AENC"_CASE:
        case "ENCR"_CASE:
        {
            php_error(E_WARNING, "Audio Encryption not supported (take your DRM bullshit elsewhere)");
            return false;
        } break;

        case "COMR"_CASE:
        {
            php_error(E_WARNING, "Commercial Frame not supported.");
            return false;
        } break;

        case "USER"_CASE:
        {
            php_error(E_WARNING, "Terms of use Frame not supported.");
            return false;
        } break;

        case "IPLS"_CASE:
        {
            php_error(E_DEPRECATED, "Please use TIPL in place of IPLS for Involved People List.");
            return false;
        } break;

        case "LINK"_CASE:
        {
            php_error(E_WARNING, "LINK Frame not supported.");
            return false;
        } break;

        case "MCDI"_CASE:
        {
            php_error(E_WARNING, "Music CD Identification Frame not supported.");
            return false;
        } break;

        case "GRID"_CASE:
        {
            php_error(E_WARNING, "Group Identification Registration not supported.");
            return false;
        } break;

        case "RBUF"_CASE:
        {
            php_error(E_WARNING, "Recommended Buffer Size (streaming) Frame not supported.");
            return false;
        } break;

        case "SYLT"_CASE: /* synchronized lyrics text */
        case "SYTC"_CASE: /* synchronized tempo codes */
        case "EQUA"_CASE: /* equalisation */
        case "RVRB"_CASE: /* reverb */
        case "ETCO"_CASE: /* event timing codes */
        case "MLLT"_CASE: /* mpeg location lookup table */
        case "PCNT"_CASE: /* play counter */
        case "POPM"_CASE: /* popularimeter */
        case "POSS"_CASE: /* position synchronisation */
        case "RVAD"_CASE: /* relative volume adjustment (replay gain) */
        {
            php_error(E_WARNING, "Frames intended for media playback (synchronized time codes or lyrics for karaoke players; popularimeter ratings, etc)\
and player personalisation (EQ, reverb, replay gain, etc...)\
are not currently supported.");
            return false;
        } break;

        case "GEOB"_CASE:
        {
            php_error(E_WARNING, "General Encapsulated Object Considered Harmful.\
Although similar enough to APIC to implement and TagLib has a ID3v2::GeneralEncapsulatedObjectFrame,\
embedded objects can cause playback issues in some players and serve no practical application for me at this point in time.");
            return false;
        } break;

        /**
         * u dun fucked up rtfm n00b xD 
         */
        default:
        {
            php_error(E_WARNING, "Invalid Frame ID.");
            return false;
        }
    }
    return true;
}

/**
 * public function setID3v2()
 */
PHP_METHOD(TagLibMPEG, setID3v2) {
    zval *newFrames;
    taglibmpegfile_object *thisobj = (taglibmpegfile_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    zend_bool overwrite_existing_tags = true;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &newFrames, &overwrite_existing_tags) == FAILURE) {
        RETURN_FALSE;
    }

    if(Z_TYPE_P(newFrames) != IS_ARRAY) {
        php_error(E_WARNING, "TagLibMPEG::setID3v2 expects associative array of FRAME_IDs as keys. See http://id3.org/id3v2.3.0#Declared_ID3v2_frames");
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
 
    if(overwrite_existing_tags) {
        TagLib::ID3v2::FrameList l = tag->frameListMap()[frameID];
            if(!l.isEmpty()) {
            for(TagLib::List<TagLib::ID3v2::Frame*>::Iterator it = l.begin(); it != l.end(); it++) {
            tag->removeFrame((*it),true);
        }
        }
    }

        if(id3v2_set_frame(tag, data, byteVector, frameID) == false) {
            RETURN_FALSE;
        }

        if(taglib_error()) {
            RETURN_FALSE;
        }
    }

    const TagLib::StringList unsupported = tag->properties().unsupportedData();
    tag->removeUnsupportedProperties(unsupported);

    if(thisobj->file->save(TagLib::MPEG::File::TagTypes::ID3v2)) {
        RETURN_TRUE;
    }

    taglib_error();
    RETURN_FALSE;
}

/**
 * See taglib.cpp for how this all comes together 
 */
static zend_function_entry php_taglibmpeg_methods[] = {
    PHP_ME(TagLibMPEG, __construct,         NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(TagLibMPEG, getAudioProperties,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, hasID3v1,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, hasID3v2,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, getID3v1,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, setID3v1,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, getID3v2,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, setID3v2,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(TagLibMPEG, stripTags,           NULL, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
