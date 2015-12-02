# taglib-php

## XXX this readme is an unfinished WIP XXX

A php extension which wraps [TagLib](http://taglib.github.io).

## Table of Contents

###### Preamble
1. [Introduction](#introduction)
2. [Installation/Configuration](#installation-configuration)
	- [Requirements](#requirements)
    - [Installation](#installation)
3. **[A note on Errors and Exceptions](#taglib-errors)**

###### API Reference

4. [TagLib](#taglib)
	- [Predefined Constants](#taglib-constants)
    - [getPictureTypeAsString()](#taglib-getpicturetypeasstring)
5. [TagLibFLAC](#taglibflac)
	- [__construct()](#taglibflac-__construct)
	- [getAudioProperties()](#taglibflac-getaudioproperties)
	- [hasID3v1()](#taglibflac-hasid3v1)
	- [getID3v1()](#taglibflac-getid3v1)
	- <s>[setID3v1()](#taglibflac-setid3v1)</s>
	- [hasID3v2()](#taglibflac-hasid3v2)
	- [getID3v2()](#taglibflac-getid3v2)
	- <s>[setID3v2()](#taglibflac-setid3v2)</s>
	- [hasXiphComment()](#taglibflac-hasxiphcomment)
	- [getXiphComment()](#taglibflac-getxiphcomment)
	- [setXiphComment()](#taglibflac-setxiphcomment)
	- [hasPicture()](#taglibflac-haspicture)
	- [getPictures()](#taglibflac-haspicture)
	- [setPicture()](#taglibflac-haspicture)
	- [stripTags()](#taglibflac-striptags)
6. [TagLibMPEG](#taglibmpeg)
	- [__construct()](#taglibmpeg-__construct)
	- [getAudioProperties()](#taglibmpeg-getaudioproperties)
	- [hasID3v1()](#taglibmpeg-hasid3v1)
	- [getID3v1()](#taglibmpeg-getid3v1)
	- [setID3v1()](#taglibmpeg-setid3v1)
	- [hasID3v2()](#taglibmpeg-hasid3v2)
	- [getID3v2()](#taglibmpeg-getid3v2)
	- [setID3v2()](#taglibmpeg-setid3v2)
	- [stripTags()](#taglibmpeg-striptags)
7. [TagLibOGG](#taglibogg)
	- [Predefined Constants](#taglibogg-constants)
	- [__construct()](#taglibogg-__construct)
	- [getAudioProperties()](#taglibogg-getaudioproperties)
	- [hasXiphComment()](#taglibogg-hasxiphcomment)
	- [getXiphComment()](#taglibogg-getxiphcomment)
	- [setXiphComment()](#taglibogg-setxiphcomment)
	- [stripTags()](#taglibogg-striptags)

###### Known Issues
1. [On ID3v2 and FLAC...](#id3v2-flac-problems)

## Introduction

At first I wanted to make a full-fledged, 1:1 extension to the entire TagLib API for php. Upon fully exploring my options in accomplishing such a thing, I found that it's too ambitious and not really necessary for what I need at this time.

[swig](http://www.swig.org) can probably accomplish it, but would involve a lot of tweaking via custom .cpp and .i files anyway so if you came here looking for that, sorry, but look into swig.

Instead, this is going to use the taglib functions in C++ and expose useful methods packaged in a class to php for specific audio formats.

Previously, I had written scripts in ruby and perl (both of which have taglib wrappers with lots of features exposed and are available right now by the way) to accomplish this. I am basing much of this on those scripts.

At this time I am planning to only support MP3, OGG, and FLAC.

All I need from taglib is to read and write tags from audio  files, preferably as stupid-simple and self-contained as possible.

## <a id="installation-configuration">Installation/Configuration</a>

\* ~ \* ~ \*

** linux only at the moment ** (not tested on BSD or OSX, or anything else for that matter)

\* ~ \* ~ \*

** PHP5 only at the moment **

\* ~ \* ~ \*

** no support for PHP7 planned yet ** (different extension API)

\* ~ \* ~ \*

** no support for HHVM planned yet ** (way different extension API)

\* ~ \* ~ \*

** no support for windows planned ever **

\* ~ \* ~ \*

# this is very much a proof-of-concept, work-in-progress, no-waranty-guaranteed, use-at-your-risk, here-be-dragons, no-good-very-bad, you-might-not-need, considered-harmful, disclaimer-of-damages-notwithstanding, etc, etc

don't use this if you're not ready to break fast and move stuff.

If you know C++ and/or have familiarity with the TagLib API and/or the PHP Extension API in addition to knowing PHP please consider contributing :3

### Requirements

1. gcc, g++, cmake, etc
2. taglib
3. php

### Installation

1. get [taglib](http://taglib.github.io) **v.1.9.1** and compile

2. get the source code for your current php version by either:
 - `apt-get source php5`
 - [official php source downloads](http://php.net/downloads.php)

3. extract the files

4. `cd /path/to/php-source-code/ext/`

5. `git clone git@github.com:generaltso/taglib-php`

6. `cd taglib-php`

6. edit `build.sh` and change the following text:
 1. `/usr/local/include/taglib` to where the taglib `.h` files maybe found.
 	- (if you don't know, something like `sudo find / -name "tstringlist.h"` might help)
 	- `sudo find /usr -name "tstringlist.h"` might be faster

 2. `/usr/local/lib/php/modules` to where your php extension directory is
    - see `extension_dir` in your `php.ini`
    - `php -i` from CLI and `<?php phpinfo(); ?>` from a web script might also be really useful.
    
7. run `./build.sh --no-tests`
	- this script will attempt to copy the built module to the extension directory you already specified, if you get a password prompt that's because it's doing exactly this:

8. `sudo cp -R modules/* /path/to/extension_dir`

9. add `extension=taglib.so` to your php.ini

10. be sure to restart apache2 (for mod_php) or restart php5-fpm if you're using that.

### To Run Tests:

From `taglib-php` directory
```bash
$ cd test
$ php -f taglib-php-tests.php | less -r
```

Please note that all audio files included with the test suite are 1 second clips of silence.

It may be advantageous to test this extension against actual audio files of a typical length with actual tags.

##### To add additional audio files to run against, you'll need the following utilities:

```bash
# apt-get install flac vorbis-tools sox libsox libsox-fmt-all libsox-fmt-mp3 mp3info id3v2 oggz
```

(*names of packages in debian, might be different for your platform, don't know where the source to all of these individually are*)

Then, from `taglib-php/test` directory
```bash
$ cp /path/to/some.mp3 testfiles/
$ cp /path/to/some.ogg testfiles/
$ # and so on..., then:
$ php -f generate-taglib-php-tests.json.php > taglib-php-tests.json
```

If contributing back, please ***don't*** include any of your own audio files or a modified `taglib-php-tests.json` file with your pull request...


### To Write Tests:

From `taglib-php/test`:
```bash
$ cd testcases
$ ls
```

Pick a file at random and prepare for eyebleed.

## <a id="taglib-errors">A note on Errors and Exceptions</a>

###### PHP Errors
The methods provided by this extension will issue PHP errors of level `E_WARNING` and return `FALSE` by default if they encounter an error.

It is therefore recommended that you have `error_reporting` visible in your development environment in order to view the error messages during development, e.g:

```php
error_reporting(E_ALL|~E_STRICT);
ini_set('display_errors', 1);
```

###### Exceptions
The one exception to this being **class constructors**, which will throw `Exception` \*

It is therefore recommended that you wrap class constructors in a `try`-`catch` block, e.g:

```php
try{
	$t = new TagLibMPEG($path_to_mp3);
} catch(Exception $e) {
	echo "Whoops! That wasn't a valid file!\n", $e->getMessage();
}
```

\* *because this is the only way I found to return `NULL` from a class constructor in a PHP extension. (`RETURN_NULL` does nothing as far as I can tell...)*

###### Segmentation fault

Also, please be aware that **there is the very real possiblity of Segmentation fault errors** occuring. 

It is a high priority of mine to fix these programming errors (which are entirely my fault) but they may be *hard to detect* for the casual PHP user who has come to expect not running into segfaults during development.

Segmentation faults will bring down the entire running PHP process immediately regardless of any preventative measures taken.

The only indication that a Segmentation fault has occurred will be in the error log for your webserver\*, or from stderr if you're using php-cli.

\* *maybe php-fpm has its own error log as well idk...*

**Please file a bug report for any Segmentation faults you come across thank you very much**. 
 - If the extension works for you except that you come across this in some PHP code you wrote, please include in your report a complete PHP script which will reliably (as in, always) reproduce the Segmentation fault.
 - If you can't load the extension, or the Segmentation fault occurs outside PHP userspace, a description will suffice.

## <a id="taglib-debugging">A note on Debugging</a>

###### gdb
You should be able to debug cli php scripts using gdb (GNU Debugger) because the build script provided should include debugging symbols.

```bash
$ gdb php
(gdb) set args -f /path/to/some.php
(gdb) run
...
(gdb) bt
```

Or something.

###### strip

If you have OCD about overly bloated binaries, you can use `strip`

## <a id="taglib">TagLib Class</a>

Really just a container for the [Predefined Constants](#taglib-constants).

**NOTE**: The other classes in this extension: [TagLibFLAC](#taglibflac), [TagLibMPEG](#taglibmpeg), and [TagLibOGG](#taglibogg) do ***not*** inherit from, extend from, implement anything from or in any other way relate to this class. All of these classes exposed to the PHP userspace are distinct, independent entities.

## <a id="taglib-constants">Predefined Constants</a>

For use with get/set ID3v2 functions in this extension:

*see also [TagLib::ID3v2::AttachedPictureFrame::Type](http://taglib.github.io/api/classTagLib_1_1ID3v2_1_1AttachedPictureFrame.html#a4b18580229d68adaefac1af09654f9a4) in [attachedpictureframe.h](http://taglib.github.io/api/attachedpictureframe_8h_source.html)*
```php
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
```

------------------------------------------------------------------------------

### <a id="taglib-getpicturetypeasstring">TagLib::getPictureTypeAsString()</a>
#### Description
Convenience method to turn the integers into their respective strings.

```php
public static getPictureTypeAsString(int $type)
```
#### Parameters
`type` - one of the above [Predefined Constants](#taglib-constants)

#### Return Values
A `string` representation of the associated constant, with each word Capitalized.

Returns `FALSE` if `$type` is not an `int` or is not in the range `0..20`.

#### Examples
```php
// example usage
echo TagLib::getPictureTypeAsString(TagLib::APIC_FRONTCOVER); // "Front Cover"
```

-------------------------------------------------------------------------------

## <a id="taglibflac">TagLibFLAC Class</a>
```php
class TagLibFLAC {
    public __construct( string $filename )
    
    public array getAudioProperties( void )
    
    public bool hasID3v1( void )
    public bool|array getID3v1( void )
    //public bool|array setID3v1( array $frames ) // disabled, always returns false
    
    public bool hasID3v2( void )
    public bool|array getID3v2( void )
    //public bool setID3v2( array $frames ) // disabled, always returns false
    
    public bool hasXiphComment( void )
    public array getXiphComment( void )
    public bool setXiphComment( array $newProperties[, bool $overwrite_existing_tags = FALSE ])
    
    public bool hasPicture( void )
    public array getPictures( void )
    public bool setPicture( array $arr )
    
    public bool stripTags( void )
}
```

--------------------------------------------------------------------------------

### <a id="taglibflac-__construct">TagLibFLAC::__construct()</a>
#### Description
Attempts to open the file path provided by `$filename`.
```php
public __construct( string $filename )
```
#### Parameters
`filename` - the file to open, **must** be a valid FLAC file and php **must** have read **and** write permissions for the file.

#### Exceptions
Throws `Exception` on error.

#### Return Values
Returns a new `TagLibFLAC` object on success or `NULL` on failure.

#### Examples
```php
// example usage
try {
	$t = new TagLibFLAC('file.flac');
    echo "File is open and ready for reading/writing tags!";
} catch(Exception $e) {
	echo "Something happened.\n", $e->getMessage();
}
```

--------------------------------------------------------------------------------

### <a id="taglibflac-getaudioproperties">TagLibFLAC::getAudioProperties()</a>
#### Description
Get information about the FLAC file.

```php
public array getAudioProperties( void )
```

#### Parameters
None

#### Return Values
An array with the following keys and possible values:

 - `length` - `int` duration of audio in seconds
 - `bitrate` - `int` bitrate of audio in kilobits per second (kbps)
 - `sampleRate` - `int` sample rate of audio in Hz
 - `channels` - `int` number of audio channels (mono = 1, stereo = 2, ...)
 - `sampleWidth` - `int` bits per sample of audio
 - `sampleFrames` - `int` number of frames in audio

#### Examples
```php
// example usage
$t = new TagLibFLAC('file.flac');
print_r($t->getAudioProperties());
/**
 * Array
 * (
 *	[length] => 1,
 *	[bitrate] => 107,
 *	[sampleRate] => 44100,
 *	[channels] => 2,
 *	[sampleWidth] => 16,
 *	[sampleFrames] => 44100
 * )
 */
```

--------------------------------------------------------------------------------

### <a id="taglibflac-hasid3v1">TagLibFLAC::hasID3v1()</a>
#### Description
Check whether file on disk has ID3v1 tag.

```php
public bool hasID3v1( void )
```
#### Parameters
None

#### Return Values
`TRUE` if file has an ID3v1 tag, `FALSE` otherwise.

#### Examples
```php
// example usage
$t = new TagLibFLAC('file.flac');
if($t->hasID3v1()) {
	// do stuff
}
```

--------------------------------------------------------------------------------

### <a id="taglibflac-getid3v1">TagLibFLAC::getID3v1()</a>
#### Description
If the file on disk has an ID3v1 tag, get the ID3v1 tag as an associative array.
```php
public bool|array getID3v1( void )
```
#### Parameters
None

#### Return Values
Returns an associative array of `string` frameIDs as keys and their `string` values.

Returns `FALSE` if file does not have an ID3v1 tag.

Also returns `FALSE` on failure.

See also [id3.org/ID3v1](http://id3.org/ID3v1) and [wikipedia](https://en.wikipedia.org/wiki/ID3#Layout) for more information about ID3v1.
#### Examples
```php
// example usage
$t = new TagLibFLAC('file_with_id3v1_tag.flac');
print_r($t->getID3v1());
/**
 * Array
 * (
 * 	[ALBUM] => album,
 * 	[ARTIST] => artist,
 * 	[COMMENT] => comment,
 * 	[DATE] => 2000,
 * 	[GENRE] => New Age,
 * 	[TITLE] => title,
 * 	[TRACKNUMBER] => 99
 * )
 */
```

--------------------------------------------------------------------------------

### <s><a id="taglibflac-setid3v1">TagLibFLAC::setID3v1()</a></s> DISABLED

#### Always returns `FALSE`. Don't use.

See also [On ID3v2 and FLAC...](#id3v2-flac-problems)

--------------------------------------------------------------------------------

### <a id="taglibflac-hasid3v2">TagLibFLAC::hasID3v2()</a>
#### Description
Check whether file on disk has ID3v2 tag.

```php
public bool hasID3v2( void )
```

#### Parameters
None

#### Return Values
`TRUE` if file has an ID3v2 tag, `FALSE` otherwise.

#### Examples
```php
// example usage
$t = new TagLibFLAC('file.flac');
if($t->hasID3v2()) {
	// do stuff
}
```

--------------------------------------------------------------------------------

### <a id="taglibflac-getid3v2">TagLibFLAC::getID3v2()</a>
#### Description
If the file on disk has an ID3v2 tag, get the ID3v2 tag as an associative array.
```php
public bool|array getID3v2( void )
```
#### Parameters
None

#### Return Values
Returns an associative array of `string` frameIDs as keys and their `string` values.

Returns `FALSE` if file does not have an ID3v2 tag.

Also returns `FALSE` on failure.

See also [id3.org/ID3v2](http://id3.org/ID3v2) and [wikipedia](https://en.wikipedia.org/wiki/ID3#Layout) for more information about ID3v2.
#### Examples

--------------------------------------------------------------------------------

### <a id="taglibflac-setid3v2">TagLibFLAC::setID3v2()</a>
#### Description
```php
public bool setID3v2( void )
```

#### Parameters
None

#### Return Values

#### Examples

--------------------------------------------------------------------------------

### <a id="taglibflac-hasxiphcomment">TagLibFLAC::hasXiphComment()</a>
#### Description
Check whether file on disk has a XiphComment.

```php
public bool hasXiphComment( void )
```
#### Parameters
None

#### Return Values
`TRUE` if the file has a XiphComment, `FALSE` otherwise.

`FALSE` on failure

#### Examples
```php
// example usage
$t = new TagLibFLAC('file.flac');
if($t->hasXiphComment()) {
	// do stuff
}
```

--------------------------------------------------------------------------------

### <a id="taglibflac-getxiphcomment">TagLibFLAC::getXiphComment()</a>
#### Description
words
```php
public bool|array getXiphComment( void )
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibflac-setxiphcomment">TagLibFLAC::setXiphComment()</a>
#### Description
words
```php
public bool setXiphComment( array $newProperties[, bool $overwrite_existing_tags = FALSE ])
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibflac-haspicture">TagLibFLAC::hasPicture()</a>
#### Description

```php
public bool hasPicture( void )
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibflac-haspicture">TagLibFLAC::getPictures()</a>
#### Description

```php
public array getPictures( void )
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibflac-haspicture">TagLibFLAC::setPicture()</a>
#### Description

```php
public bool setPicture( array $arr )
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibflac-striptags">TagLibFLAC::stripTags()</a>
#### Description
words
```php
public bool stripTags( void )
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```


## <a id="taglibmpeg">TagLibMPEG Class</a>
```php
class TagLibMPEG {
    public __construct( string $filename )
    
    public array getAudioProperties( void )
    
    public bool hasID3v1( void )
    public bool|array getID3v1( void )
    public bool|array setID3v1( array $frames )
    
    public bool hasID3v2( void )
    public bool|array getID3v2( void )
    public bool setID3v2( array $frames )
    
    public bool stripTags( void )
}
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-__construct">TagLibMPEG::__construct()</a>
#### Description
Attempts to open the file path provided by `$filename`.
```php
public __construct( string $filename )
```
#### Parameters
`filename` - the file to open, **must** be a valid MP3 file and php **must** have read **and** write permissions for the file.

May work just fine with other MPEG types (.mpeg, .mp2, etc) but this is untested.

#### Exceptions
Throws `Exception` on error.

#### Return Values
Returns a new `TagLibMPEG` object on success, `NULL` on failure.

#### Examples
```php
// example usage
try {
	$t = new TagLibMPEG('file.mp3');
    echo "File is open and ready for reading/writing tags!";
} catch(Exception $e) {
	echo "Something happened.\n", $e->getMessage();
}
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-getaudioproperties">TagLibMPEG::getAudioProperties()</a>
#### Description
Get information about the MP3 file.
```php
public array getAudioProperties( void )
```
#### Parameters
None

#### Return Values
An array with the following keys and possible values:

 - `length` - `int` duration of audio in seconds
 - `bitrate` - `int` bitrate of audio in kilobits per second (kbps)
 - `sampleRate` - `int` samplerate of audio in Hz
 - `channels` - `int` number of audio channels (mono = 1, stereo = 2, ...)
 - `version` - `string` one of the following possible values:
 	- `"MPEG Version 1"`
 	- `"MPEG Version 2"`
 	- `"MPEG Version 2.5"`
 	- `"Unknown"`
 - `channelMode`- `string` one of the following possible values:
 	- `"Mono"`
 	- `"Dual Mono"`
 	- `"Stereo"`
 	- `"Unknown"`
 - `layer` - `int`, e.g. MP3 is MPEG layer 3
 - `protectionEnabled` - `bool`
 - `isCopyrighted` - `bool`
 - `isOriginal` - `bool`

#### Examples
```php
// example usage
$t = new TagLibMPEG('file.mp3');
print_r($t->getAudioProperties());
/**
 * Array
 * (
 * 	[length] => 1234,
 * 	[bitrate] => 320,
 * 	[sampleRate] => 44100,
 * 	[channels] => 2,
 * 	[version] => MPEG Version 1,
 * 	[channelMode] => Stereo,
 * 	[layer] => 3
 * 	[protectionEnabled] =>
 * 	[isCopyrighted] =>
 * 	[isOriginal] => 1
 * )
 */
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-hasid3v1">TagLibMPEG::hasID3v1()</a>
#### Description
Check whether file on disk has ID3v1 tag.

```php
public bool hasID3v1( void )
```
#### Parameters
None

#### Return Values
`TRUE` if file has an ID3v1 tag, `FALSE` otherwise.

#### Examples
```php
// example usage
$t = new TagLibMPEG('file.mp3');
if($t->hasID3v1()) {
	// do stuff
}
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-getid3v1">TagLibMPEG::getID3v1()</a>
#### Description
If the file on disk has an ID3v1 tag, get the ID3v1 tag as an associative array.
```php
public bool|array getID3v1( void )
```
#### Parameters
None

#### Return Values
Returns an associative array of `string` frameIDs as keys and their `string` values.

Returns `FALSE` if file does not have an ID3v1 tag.

Also returns `FALSE` on failure.

See also [id3.org/ID3v1](http://id3.org/ID3v1) and [wikipedia](https://en.wikipedia.org/wiki/ID3#Layout) for more information about ID3v1.
#### Examples
```php
// example usage
$t = new TagLibMPEG('file_with_id3v1_tag.mp3');
print_r($t->getID3v1());
/**
 * Array
 * (
 * 	[ALBUM] => album,
 * 	[ARTIST] => artist,
 * 	[COMMENT] => comment,
 * 	[DATE] => 2000,
 * 	[GENRE] => New Age,
 * 	[TITLE] => title,
 * 	[TRACKNUMBER] => 99
 * )
 */
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-setid3v1">TagLibMPEG::setID3v1()</a>
#### Description
Write ID3v1 tag fields to file.
```php
public bool|array setID3v1( array $frames[, bool $overwrite_existing_tags = TRUE ] )
```
#### Parameters
`$frames` is an associative `array` of `string` values with UPPERCASE `string` frame IDs as keys e.g.:
```php
$frames = ['ALBUM' => 'Abbey Road', 'ARTIST' => 'The Beatles'];
```
`$overwrite_existing_tags`
 - if `TRUE` (default), tags already present in the file with frame IDs matching the keys of `$frames` will be overwritten with the values passed in.
 - if `FALSE`, tags already present will **not be overwritten** and will be **returned** by this function.

##### Caveats
[TagLib will truncate values greater than 28-30 bytes](http://taglib.github.io/api/classTagLib_1_1ID3v1_1_1Tag.html#details). See also [id3.org/ID3v1](http://id3.org/ID3v1) and [wikipedia](https://en.wikipedia.org/wiki/ID3#Layout) for more information about ID3v1.

Additionally, this function will issue `E_WARNING` errors and return `FALSE` if `$frames` is any other variable type than the one described here.

In other words, this function will **not do any type conversion** but instead issue PHP errors and return `FALSE` if it encounters any unexpected type.

So for something like TRACKNUMBER, 
**ALWAYS convert to string**:
```php
$tracknumber = 99;
$frames['TRACKNUMBER'] = sprintf("%d", $tracknumber);
```
**NEVER:**
```php
$tracknumber = 99;
$frames['TRACKNUMBER'] = $tracknumber; // will cause errors!
```

#### Return Values
Returns `TRUE` on success.

Returns any values from `$frames` which could not be set as an associative `array` of `string` values.

Returns `FALSE` on failure.

#### Examples
```php
// example usage
$t = new TagLibMPEG('file.mp3');
$frames = ['ALBUM' => 'Abbey Road', 'ARTIST' => 'The Beatles'];
$ret = $t->setID3v1($frames);
if($ret === true) {
	echo "Tag set successfully!";
} elseif(is_array($ret)) {
	echo "Failed to set these values:\n", print_r($ret,1);
    /**
     * Array
     * (
     * 	[ALBUM] => 'Abbey Road'
     * )
     */
} else {
	echo "Something happened."
}
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-hasid3v2">TagLibMPEG::hasID3v2()</a>
#### Description
Check whether file on disk has ID3v2 tag.

```php
public bool hasID3v2( void )
```
#### Parameters
None

#### Return Values
`TRUE` if file has an ID3v2 tag, `FALSE` otherwise.

#### Examples
```php
// example usage
$t = new TagLibMPEG('file.mp3');
if($t->hasID3v2()) {
	// do stuff
}
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-getid3v2">TagLibMPEG::getID3v2()</a>
#### Description
If the file on disk has an ID3v2 tag, get the ID3v2 tag as an array.
```php
public bool|array getID3v2( void )
```
#### Parameters
None

#### Return Values
Returns an array with the following structure:
```php
[
	0 =>	[
				"frameID" => "TPE1",
				"data"	=> "Artist Name"
			],
    //1 => ...
]
```

See [id3.org](http://id3.org/id3v2.3.0) for possible frameIDs.

It is this way because an ID3v2 tag *can* contain *multiple* frames *with the same frameID*.

Although I would urge you not to treat this unfortunate fact as a feature.

Additionally, certain ID3v2 frames contain additional data, such as APIC (Attached Picture Frame aka cover art aka an image). See example usage.

Returns `FALSE` if file does not have an ID3v2 tag.

Returns `FALSE` on failure.

#### Examples
```php
// example usage
$t = new TagLibMPEG('file.mp3');
$id3v2 = $t->getID3v2();
print_r($id3v2);
/**
 * Array(
 * 	0 => Array(
 *		[frameID] => TPE1,
 *		[data] => Artist Name
 * 	),
 * 	1 => Array(
 *		[frameID] => TIT2,
 *		[data" => Track Title
 * 	),
 * 	2 => Array(
 *		[frameID] => APIC,
 *		[data] => (base64 encoded data that will likely flood your screen),
 *		[mime] => image/jpeg,
 *		[type] => 3
 *		[desc] => Optional Description of Image
 * 	)
 * )
 */

if(!is_array($id3v2)) return;

// you can do something like this to work with this data:
$tags = [];
foreach($id3v2 as $frame) {
	$id = $frame['frameID'];
    $data = $frame['data'];
    $human_readable_id = '';
    $human_readable_data = '';
    switch($id) {
    case 'APIC':
    	$human_readable_id = 'Picture';
        
        // write the file to disk...
        // get filetype by reading $frame['mime']
    	$ext = 'unknown';
    	switch($frame['mime']) {
        case 'image/jpeg':
        	$ext = 'jpg';
            break;
        case 'image/png':
        	$ext = 'png';
			break;
        // most album art will be png or jpg
        // it is possible that any other image format might be here tho
        // in that case add more cases.
        default:
        	throw new Exception('this shouldn\'t happen');
        }
        // optionally get picture type if you care
        switch($frame['type']) {
        case TagLib::APIC_COLOUREDFISH:
            echo "<°))))><";
            break;
        // add the other APIC_* constants here if you want
        default:
            echo "(boring image)";
        }
        // write the data to a file
        $filename = "album_art$ext";
    	file_put_contents($filename, base64_decode($data));
		$human_readable_data = $filename
        break;
    // currently APIC is the only different case, structurally speaking
    // but more may prove necessary in future
	case 'TPE1':
    	$human_readable_id = 'Artist';
        $human_readable_data = $data;
        break;
	case 'TIT2':
    	$human_readable_id = 'Title';
        $human_readable_data = $data;
        break;
    // etc...
    }
    
    $tags[$human_readable_id] = $human_readable_data;
}

print_r($tags);
/**
 * (actually useful tag info hopefully)
 */
```

See also [Predefined Constants](#predefined-constants) for possible Attached Picture Frame Types.

And again, see [id3.org](http://id3.org/id3v2.3.0) for possible frameIDs.

--------------------------------------------------------------------------------

### <a id="taglibmpeg-setid3v2">TagLibMPEG::setID3v2()</a>
#### Description
words
```php
public bool setID3v2( array $frames )
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibmpeg-striptags">TagLibMPEG::stripTags()</a>
#### Description
Clear all tag information from file.
```php
public bool stripTags( void )
```
#### Parameters
None

#### Return Values
Returns `TRUE` on success, `FALSE` on failure.

#### Examples
```php
// example usage
$t = new TagLibMPEG('file.mp3');
// say for argument's sake this file has both ID3v1 and ID3v2 tags.
echo $t->hasID3v1() ? 'true' : 'false'; // true
echo $t->hasID3v2() ? 'true' : 'false'; // true

$t->stripTags();

// now it has neither
echo $t->hasID3v1() ? 'true' : 'false'; // false
echo $t->hasID3v2() ? 'true' : 'false'; // false
```

TagLib provides ways to remove individual tags without affecting the other(s) but this causSegmentation fault

## <a id="taglibogg">TagLibOGG</a>
```php
class TagLibOGG {
    public __construct( string $filename[, int $type = TagLibOGG::VORBIS ])
    
    public array getAudioProperties( void )
    
    public bool hasXiphComment( void )
    public array getXiphComment( void )
    public bool setXiphComment( array $newProperties[, bool $overwrite_existing_tags = FALSE ])
    
    public bool stripTags( void )
}
```

## <a id="taglibogg-constants">Predefined Constants</a>
```php
TagLibOGG::VORBIS = 1
TagLibOGG::OPUS   = 2
TagLibOGG::FLAC   = 3
TagLibOGG::SPEEX  = 4 // don't use this one
```

--------------------------------------------------------------------------------

### <a id="taglibogg-__construct">TagLibOGG::__construct()</a>
#### Description
Attempts to open the specified file path.
```php
public __construct( string $filename[, int $codec = TagLibOGG::VORBIS ])
```

#### Parameters
`filename` - the file to open, **must** be a valid OGG file of `codec` and php **must** have read **and** write permissions for the file.
`codec` - `int` codec used in the OGG container

***Most OGGs are vorbis***, which is also the **default** for `$codec`, so you probably don't need to worry about this

But OGG is actually a container which supports multiple codecs. If your OGG is a different codec, you must specify it in the constructor with one of the [Predefined Constants](#taglibogg-constants).

Except speex because [speex is deprecated](http://speex.org/).

#### Exceptions
Throws `Exception` on error.

#### Return Values
Returns a new `TagLibOGG` object on success or `NULL` on failure.

#### Examples
```php
// example usage
try {
	$t = new TagLibOGG('file.ogg');
    echo "File is open and ready for reading/writing tags!";
} catch(Exception $e) {
	echo "Something happened.\n", $e->getMessage();
}
```

--------------------------------------------------------------------------------

### <a id="taglibogg-getaudioproperties">TagLibOGG::getAudioProperties()</a>
#### Description
Get Information about the OGG.
```php
public array getAudioProperties( void )
```

#### Parameters
None

#### Return Values
An array with the following keys and possible values, *depending on the codec of the OGG*:

1. **Common to all codecs**:
 - `length` - `int` duration of audio in seconds
 - `bitrate` - `int` bitrate of audio in kilobits per second (kbps)
 - `sampleRate` - `int` samplerate of audio in Hz
 - `channels` - `int` number of audio channels (mono = 1, stereo = 2, ...)

2. `TagLibOGG::VORBIS` only:
 - `vorbisVersion` - `int` Returns the Vorbis version, currently "0" (as specified by the spec).
 - `bitrateMaximum` - `int` maximum bitrate
 - `bitrateNominal` - `int` nominal bitrate
 - `bitrateMinimum` - `int` minimum bitrate

3. `TagLibOGG::OPUS` only:
 - `opusVersion` - `int` Returns the Opus version, in the range 0...255.
 - `inputSampleRate` - `int` The Opus codec supports decoding at multiple sample rates, there is no single sample rate of the encoded stream. This returns the sample rate of the original audio stream.

4. `TagLibOGG::FLAC` only:
 - `sampleWidth` - `int` bits per sample of audio
 - `sampleFrames` - `int` number of frames in audio



#### Examples
```php
// vorbis
$v = new TagLibOGG('vorbis.ogg', TagLibOGG::VORBIS);
print_r($v->getAudioProperties());
/**
 * Array(
 * 	[length] => 1,
 * 	[bitrate] => 120,
 * 	[sampleRate] => 44100,
 * 	[channels] => 1,
 * 	[vorbisVersion] => 0,
 * 	[bitrateMaximum] => 0,
 * 	[bitrateNominal] => 120000,
 * 	[bitrateMinimum] => 0
 * )
 */
 
// opus
$o = new TagLibOGG('opus.ogg', TagLibOGG::OPUS);
print_r($o->getAudioProperties());
/**
 * Array(
 * 	[length] => 1,
 * 	[bitrate] => 120,
 * 	[sampleRate] => 44100,
 * 	[channels] => 1,
 * 	[opusVersion] => 2,
 * 	[inputSampleRate] => 44100
 * )
 */
 
// flac
$f = new TagLibOGG('flac.ogg', TagLibOGG::FLAC);
print_r($f->getAudioProperties());
/**
 * Array(
 * 	[length] => 1,
 * 	[bitrate] => 120,
 * 	[sampleRate] => 44100,
 * 	[channels] => 1,
 *	[sampleWidth] => 16,
 *	[sampleFrames] => 44100
 * )
 */
```

--------------------------------------------------------------------------------

### <a id="taglibogg-hasxiphcomment">TagLibOGG::hasXiphComment()</a>
#### Description
Check whether file on disk has a XiphComment.

```php
public bool hasXiphComment( void )
```
#### Parameters
None

#### Return Values
`TRUE` if the file has a XiphComment, `FALSE` otherwise.

`FALSE` on failure

#### Examples
```php
// example usage
$t = new TagLibOGG('file.ogg');
if($t->hasXiphComment()) {
	// do stuff
}
```

--------------------------------------------------------------------------------

### <a id="taglibogg-getxiphcomment">TagLibOGG::getXiphComment()</a>
#### Description
Get the XiphComment as an associative array.

```php
public bool|array getXiphComment( void )
```

#### Parameters
None

#### Return Values

#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibogg-setxiphcomment">TagLibOGG::setXiphComment()</a>
#### Description
words
```php
public bool setXiphComment( array $newProperties[, bool $overwrite_existing_tags = FALSE ])
```
#### Parameters
#### Return Values
#### Examples
```php
// example usage
```

--------------------------------------------------------------------------------

### <a id="taglibogg-striptags">TagLibOGG::stripTags()</a>
#### Description
Clear XiphComment from file.
```php
public bool stripTags( void )
```
#### Parameters
None

#### Return Values
Returns `TRUE` on success, `FALSE` on failure.

#### Examples
```php
// example usage
$t = new TagLibOGG('file.ogg');
// say for argument's sake this file has a XiphComment.
echo $t->hasXiphComment() ? 'true' : 'false'; // true

$t->stripTags();

// now it doesn't
echo $t->hasXiphComment() ? 'true' : 'false'; // false
```

--------------------------------------------------------------------------------

# Known Issues

## <a id="id3v2-flac-problems">On ID3v2 and FLAC...</a>

