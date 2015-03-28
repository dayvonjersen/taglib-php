#taglib wrapper for php

 - http://taglib.github.io

 - http://php.net

right now it doesn't do much, but
##update Fri 27 Mar 2015 10:19:44 PM EDT
 - we can get, set, and clear ID3v2 tags from MP3s. I am excited.

###[please submit bugs/feature requests/feedback here](https://meta.bunzilla.ga/report/category/14)

---

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

At this time I am planning to support MP3, OGG, and FLAC. Here is some 
sample pseudo-php for what this will (hopefully, maybe) turn out like:

    class TagLibMPEG {

        public function __construct( string $fileName );

        public function getAudioProperties() {
            $audioProperties = [
                'length' => (int) "some length...",
                'bitrate' => (int) "some bitrate..."
            ];
            return $audioProperties;

            // yes I know I'm doing awful type conversions for no reason
            // yes I will document what the returned array values will be
            // yes I'm aware I'm commenting in what isn't even real code
        }

        public function getID3v2() {
            $tag = [
                'TPE1' => 'This is the artist field in an ID3v1 or v2 tag',
                'TIT2' => 'This is the title; maybe I'll make a mapping',
                'TALB' => 'so you can convert these to readable/useful strings',
                'COMM' => 'that was an album btw and this is a comment'
                // etc...
            ];
            return $tag;
        }

        public function setID3v2( array $fields = ['FRAME_ID' => "New Value"] ) {
            // writes stuff to the file
            // refer to the id3 spec: http://id3.org/id3v2.3.0
            // some frames require an array argument 
            // TODO: better documentation :x
        }

        public function clearID3v2() {
            // nuke the whole tag
            // returns true on success
        }

    }

I mean that's all I need from taglib: to read and write tags from audio 
files, preferably as stupid-simple and self-contained as possible.

I don't want to be messing with ByteVectors and casting things to and from 
TagLib::String while juggling the practically undocumented PHP internals
[inb4 someone links me to the php internals](http://php.net/manual/en/internals2.php)
just to expose the entire API to php. Ain't nobody got time for that.
