# taglib-php

A php extension which wraps [TagLib](http://taglib.github.io).

## Table of Contents

1. [Introduction](#introduction)
2. [Installation/Configuration](#installation/configuration)
	- Requirements
    - Installation
    - Runtime Configuration
    - Resource Types
3. [Predefined Constants](#predefined-constants)
4. TagLib
5. TagLibFLAC
6. [TagLibMPEG](#taglibmpeg-class)
	- [__construct()](#taglibmpeg-__construct)
	- [getAPE()](#taglibmpeg-getAPE)
	- [getAudioProperties()](#taglibmpeg-getaudioproperties)
	- [getID3v1()](#taglibmpeg-getid3v1)
	- [getID3v2()](#taglibmpeg-hasid3v2)
	- [hasAPE()](#taglibmpeg-hasape)
	- [hasID3v1()](#taglibmpeg-hasid3v1)
	- [hasID3v2()](#taglibmpeg-hasid3v2)
	- [setID3v1()](#taglibmpeg-setid3v1)
	- [setID3v2()](#taglibmpeg-setid3v2)
	- [stripTags()](#taglibmpeg-striptags)
7. [TagLibOGG](#taglibogg-class)
	- [__construct()](#taglibogg-__construct)
	- [getAudioProperties()](#taglibogg-getaudioproperties)
	- [hasXiphComment()](#taglibogg-hasxiphcomment)
	- [getXiphComment()](#taglibogg-getxiphcomment)
	- [setXiphComment()](#taglibogg-setxiphcomment)

## Introduction

At first I wanted to make a full-fledged, 1:1 extension to the entire TagLib API for php. Upon fully exploring my options in accomplishing such a thing, I found that it's too ambitious and not really necessary for what I need at this time.

[swig](http://www.swig.org) can probably accomplish it, but would involve a lot of tweaking via custom .cpp and .i files anyway so if you came here looking for that, sorry, but look into swig.

Instead, this is going to use the taglib functions in C++ and expose useful methods packaged in a class to php for specific audio formats.

Previously, I had written scripts in ruby and perl (both of which have taglib wrappers with lots of features exposed and are available right now by the way) to accomplish this. I am basing much of this on those scripts.

At this time I am planning to only support MP3, OGG, and FLAC.

All I need from taglib is to read and write tags from audio  files, preferably as stupid-simple and self-contained as possible. 

## Installation/Configuration

### Requirements

1. gcc, cmake, etc
2. taglib
3. php

### Installation

1. get [taglib](http://taglib.github.io) and compile
2. get [php source](http://php.net/downloads.php) (or `apt-get source php5`) for your current php version (optionally compile a new version of php from source)
3. `cd /path/to/php-source-code/ext/`
4. `git clone git@github.com:generaltso/taglib-php`
5. `cd taglib-php`
6. run `./build.sh --no-tests`
7. `sudo cp modules/taglib.so /path/to/extension_dir` (see `extension_dir` in your `php.ini`)
8. add `extension=taglib.so` to your php.ini

### Runtime Configuration

None

### Resource Types

What

## Predefined Constants

For use with TagLibMPEG::stripTags() in this extension:

    TagLib::STRIP_NOTAGS  = 0x0000;
    TagLib::STRIP_ID3V1   = 0x0001;
    TagLib::STRIP_ID3V2   = 0x0002;
    TagLib::STRIP_APE     = 0x0004;
    TagLib::STRIP_ALLTAGS = 0xffff;
*see also TagLib::MPEG::File::TagTypes in mpegfile.h *

For use with get/set ID3v2 functions in this extension:

    TagLib::APIC_OTHER              = 0x00
    TagLib::APIC_FILEICON           = 0x01
    TagLib::APIC_OTHERFILEICON      = 0x02
    TagLib::APIC_FRONTCOVER         = 0x03
    TagLib::APIC_BACKCOVER          = 0x04
    TagLib::APIC_LEAFLETPAGE        = 0x05
    TagLib::APIC_MEDIA              = 0x06
    TagLib::APIC_LEADARTIST         = 0x07
    TagLib::APIC_ARTIST             = 0x08
    TagLib::APIC_CONDUCTOR          = 0x09
    TagLib::APIC_BAND               = 0x0A
    TagLib::APIC_COMPOSER           = 0x0B
    TagLib::APIC_LYRICIST           = 0x0C
    TagLib::APIC_RECORDINGLOCATION  = 0x0D
    TagLib::APIC_DURINGRECORDING    = 0x0E
    TagLib::APIC_DURINGPERFORMANCE  = 0x0F
    TagLib::APIC_MOVIESCREENCAPTURE = 0x10
    TagLib::APIC_COLOUREDFISH       = 0x11
    TagLib::APIC_ILLUSTRATION       = 0x12
    TagLib::APIC_BANDLOGO           = 0x13
    TagLib::APIC_PUBLISHERLOGO      = 0x14

*see also TagLib::ID3v2::AttachedPictureFrame::Type in attachedpictureframe.h *
## TagLib Class

Just a container for the above constants.

## TagLibFLAC Class

Nothing to see here (yet)

## TagLibMPEG Class
```php
	TagLibMPEG {
    	public __construct( string $filename[, bool $readProperties = TRUE ])
        public bool|array getAPE( void )
        public array getAudioProperties( void )
        public bool|array getID3v1( void )
        public bool|array getID3v2( void )
        public bool hasAPE( void )
        public bool hasID3v1( void )
        public bool hasID3v2( void )
        public bool|array setID3v1( array $frames )
        public bool setID3v2( array $frames )
        public bool stripTags([ int $tags = TagLib::STRIP_ALLTAGS ])
    }
```
### <a id="taglibmpeg-__construct">TagLibMPEG::__construct()</a>

```php
public __construct( string $filename[, bool $readProperties = TRUE ])
```
Attempts to open the file path provided by `$filename`. Setting `$readProperties` to `FALSE` will probably break getAudioProperties or something.

### <a id="taglibmpeg-getape">TagLibMPEG::getAPE()</a>
```php
public bool|array getAPE( void )
```
### <a id="taglibmpeg-getaudioproperties">TagLibMPEG::getAudioProperties()</a>
```php
public array getAudioProperties( void )
```
### <a id="taglibmpeg-getid3v1">TagLibMPEG::getID3v1()</a>
```php
public bool|array getID3v1( void )
```
### <a id="taglibmpeg-getid3v2">TagLibMPEG::getID3v2()</a>
```php
public bool|array getID3v2( void )
```
### <a id="taglibmpeg-hasape">TagLibMPEG::hasAPE()</a>
```php
public bool hasAPE( void )
```
### <a id="taglibmpeg-hasid3v1">TagLibMPEG::hasID3v1()</a>
```php
public bool hasID3v1( void )
```
### <a id="taglibmpeg-hasid3v2">TagLibMPEG::hasID3v2()</a>
```php
public bool hasID3v2( void )
```
### <a id="taglibmpeg-setid3v1">TagLibMPEG::setID3v1()</a>
```php
public bool|array setID3v1( array $frames )
```
### <a id="taglibmpeg-setid3v2">TagLibMPEG::setID3v2()</a>
```php
public bool setID3v2( array $frames )
```
### <a id="taglibmpeg-striptags">TagLibMPEG::stripTags()</a>
```php
public bool stripTags([ int $tags = TagLib::STRIP_ALLTAGS ])
```

## TagLibOGG Class
```php
TagLibOGG {
    public __construct( string $filename[, int $type = TagLibOGG::VORBIS ])
    public array getAudioProperties( void )
    public array getXiphComment( void )
    public bool setXiphComment( array $newProperties[, bool $overwrite_existing_tags = FALSE ])
}
```
### <a id="taglibogg-__construct">TagLibOGG::__construct()</a>
```php
public __construct( string $filename[, int $type = TagLibOGG::VORBIS ])
```
### <a id="taglibogg-getaudioproperties">TagLibOGG::getAudioProperties()</a>
```php
public array getAudioProperties( void )
````
### <a id="taglibogg-hasxiphcomment">TagLibOGG::hasXiphComment()</a>
```php
public array hasXiphComment( void )
```
### <a id="taglibogg-getxiphcomment">TagLibOGG::getXiphComment()</a>
```php
public array getXiphComment( void )
```

### <a id="taglibogg-setXiphComment">TagLibOGG::setXiphComment()</a>
```php
public bool setXiphComment( array $newProperties[, bool $overwrite_existing_tags = FALSE ])
```
