#taglib wrapper for php

 - http://taglib.github.io

 - http://php.net

right now it doesn't do much, but
##update Fri 27 Mar 2015 10:19:44 PM EDT

 - we can get, set, and clear ID3v2 tags from MP3s. I am excited.

##update Sat 28 Mar 2015 02:41:55 PM EDT

 - we can get, set, and clear ID3v1 tags
 - exposed hasID3v1Tag, has ID3v2Tag, hasAPETag
 - abstracted clear(ID3v1|ID3v2|APE) to stripTags(TagLibMPEG::STRIP_\*)
 - APETag can be read or cleared only

##update Sat 28 Mar 2015 07:41:47 PM EDT

 - OGG Support! (except for speex because [speex is ded](http://www.speex.org/))
 - Need to write more/better tests and update this documentation 

###[please submit bugs/feature requests/feedback here](https://meta.bunzilla.ga/report/category/14)

---

#author's note

 - I might use the terms "Tag", "Frame", "Field", and "Property" interchangably in my explanations below in a somewhat counterintuitive attempt to simplify the usage of this extension. 

 - TagLib uses these terms consistently, so if you're confused or want to know the specific implemenation details, head over to http://taglib.github.io/api for better descriptions.

 - While you need to use "ARTIST" in an ID3v1 tag and "TPE1" in an ID3v2 tag with this extension and TagLib has the `artist()` method for each Tag to do this seemlessly behind the scenes, I have deliberately chosen not to expose these methods to PHP at this time.
    - My reasoning for this is that I plan to use this to read **all** of the tagging metadata from an audio file, process it, destroy all of the metadata, and rebuild it according to a specification I have determined in order to have fully consistent audio files with no extraneous tags that might have come from other taggers and certain specific tags (such as ID3v2's TCOP and WCOP) for which (rightly) no such abstractions exist in TagLib. 
    - Going the other way and abstracting *every possible ID3v2 tag* is beyond my threshold for pain.

#a note about errors

 - right now this triggers standard php_errors (and returns false, null or just immediately) when it encounters a problem

I'm considering and looking into throwing exceptions but maybe it is better this way as most errors will be encountered when the proper precautions haven't been made

e.g. if you try to read a file that doesn't exist:

`try {`

`  $mp3 = new TagLibMPEG('/file/that/does/not/exist');`

`} catch($e Exception) {`

`  echo 'No such file!';`

`}`

seems like an anti pattern since when dealing with files you would usually do something like

`if(file_exists($file) && (new finfo(FILEINFO_MIME_TYPE))->file($file) == 'audio/mpeg') {`

`// note there are multiple valid mimetypes for mp3 other than audio/mpeg`

`   $mp3 = new TagLibMPEG($file);`

`}`

since you normally wouldn't attempt to open a file that wasn't valid under any circumstance

#what gets exposed to php
(as of Sat 28 Mar 2015 02:48:22 PM EDT)

`class TagLibMPEG`

###constants

for use with TagLibMPEG::stripTags()

`TagLibMPEG::STRIP_NOTAGS = 0`

`TagLibMPEG::STRIP_ID3V1 = 1`

`TagLibMPEG::STRIP_ID3V2 = 2`

`TagLibMPEG::STRIP_APE = 4`

`TagLibMPEG::STRIP_ALLTAGS = 65535`


for use with APIC ID3v2 tag (attached picture frame aka "Album Art")

`TagLibMPEG::APIC_OTHER = 0`

`TagLibMPEG::APIC_FILEICON = 1`

`TagLibMPEG::APIC_OTHERFILEICON = 2`

`TagLibMPEG::APIC_FRONTCOVER = 3`

`TagLibMPEG::APIC_BACKCOVER = 4`

`TagLibMPEG::APIC_LEAFLETPAGE = 5`

`TagLibMPEG::APIC_MEDIA = 6`

`TagLibMPEG::APIC_LEADARTIST = 7`

`TagLibMPEG::APIC_ARTIST = 8`

`TagLibMPEG::APIC_CONDUCTOR = 9`

`TagLibMPEG::APIC_BAND = 10`

`TagLibMPEG::APIC_COMPOSER = 11`

`TagLibMPEG::APIC_LYRICIST = 12`

`TagLibMPEG::APIC_RECORDINGLOCATION = 13`

`TagLibMPEG::APIC_DURINGRECORDING = 14`

`TagLibMPEG::APIC_DURINGPERFORMANCE = 15`

`TagLibMPEG::APIC_MOVIESCREENCAPTURE = 16`

`TagLibMPEG::APIC_COLOUREDFISH = 17`

`TagLibMPEG::APIC_ILLUSTRATION = 18`

`TagLibMPEG::APIC_BANDLOGO = 19`

`TagLibMPEG::APIC_PUBLISHERLOGO = 20`

##methods

`public TagLibMPEG __construct( string FileName )`
 - attempts to open `FileName` and when called with `new` gives you a TagLibMPEG object.
 - if permissions are screwy or this is not a valid MPEG file, it will fail.
 - I haven't tested with MPEG (the video format), MP1, or MP2 and I don't know if I have any of those on hand...

`public array getAudioProperties( void )`
 - will return an assocative array of file properties such as bitrate in kbps and length in seconds

`public bool hasID3v1( void )`

`public bool hasID3v2( void )`

`public hasAPE( void )`
 - will return true if the file has the respective tag`

`public array|bool getID3v1( void )`
 - returns an associative array of tags currently in the file
 - see http://taglib.github.io/api/classTagLib_1_1PropertyMap.html#details for more information
 - returns false on failure

`public array|bool setID3v1( array newProperties, bool overwriteExisting = false )`
 - newProperties is an associative array of TAGNAME => 'text'
 - see the above link for valid tags
 - note that ID3v1 is limited to 28-30 bytes per field

 - will attempt to write the new tags to the file, so this is designed for a batch job
 - e.g. make your array of new tags and pass it to this function, don't call it one tag at a time if you can avoid it

 - returns false on failure
 - returns array of tags which **were not set** in the event you pass invalid tag names
 - returns true if tags were all set
 - so you will have to test for what happened by capturing the return value

`public array|bool getID3v2( void )`
 - returns associative array of ID3v2 tag frames or false on failure
 - see http://id3.org/id3v2.3.0#Declared_ID3v2_frames 

`public bool setID3v2( array newFrames )`
 - `newFrames` is an associative array of FRAME_IDs using the above linked ID3v2 FrameIDs
 - **NOTE**: I haven't finished all of the possible FRAME_IDs yet.
 - the values of each element of the array depend on the Frame, of note:

 - T\*\*\* frames are string values
 - APIC (AttachedPictureFrame) expects a specific array argument, e.g.: 

`['data' => file_get_contents($newPicture),`

` 'mime' => 'image/jpeg', `

` 'type' => TagLibMPEG::APIC_FRONTCOVER,` 

` 'desc' => 'optional description']`

 - TXXX (User-defined TextIdentificationFrame or something) also requires an array:

`['desc' => 'Description of Custom Field', `

`'text' => 'The contents of that field']`

 - Will trigger E_WARNING if you pass invalid arguments with a (hopefully) helpful hint

`public bool stripTags( int tagtypes = TagLibMPEG::STRIP_ALL )`
 - will nuke the tags provided, which you can bitwise OR together from the provided TagLibMPEG::_STRIP_\* constants.
 - default TagLib behavior of stripping all tags when no argument is passed.

---

## about

At first I wanted to make a full-fledged, 1:1 extension to the entire
TagLib API for php. Upon fully exploring my options in accomplishing such a
thing, I found that it's too ambitious and not really necessary for what I
need at this time.

[swig](http://www.swig.org) can probably accomplish it, but would involve a lot
of tweaking via custom .cpp and .i files anyway so if you came here looking for
that, sorry, but look into swig.

Instead, this is going to use the taglib functions in C++ and expose useful
methods packaged in a class to php for specific audio formats.

Previously, I had written scripts in ruby and perl (both of which have
taglib wrappers with lots of features exposed and are available right now
by the way) to accomplish this. I am basing much of this on those scripts.

At this time I am planning to only support MP3, OGG, and FLAC.

All I need from taglib is to read and write tags from audio 
files, preferably as stupid-simple and self-contained as possible.


