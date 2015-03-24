/**
 * TagLib implementation and php extension
 * Mostly for manipulating Tags
 * But also reading audioProperties
 * Aiming to implement MPEG (MP3), FLAC, and OGG */

/**
 * standard libs */
#include <iostream>
#include <iomanip>
#include <stdio.h>

/**
 * TagLib libs 
 * - Currently using these environment variables to compile:
 *     C_INCLUDE_PATH="/usr/local/include/taglib"
 *     CPLUS_INCLUDE_PATH="/usr/local/include/taglib"
 *
*** XXX working on it
******************************************************
#include <fileref.h>
#include <tag.h>
#include <id3v1tag.h>
********************************************
***/

/**
 * .h for this file (taglib.cpp) */
#include "php_taglib.h"

#include "taglibmpeg.cpp"
