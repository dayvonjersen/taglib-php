<?php
function Test__TagLibMPEG__setID3v1($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = true
    $t = new TagLibMPEG($tmpfile);
    $original_tags = $t->getID3v1();
    if($original_tags === false) {
        $original_tags = ['ALBUM' => '', 'ARTIST' => '', 'COMMENT' => ''];
    }
    $original_tags['TITLE'] = 'testing';
    $retval = $t->setID3v1(['title'=>'testing'], true);
    assert($retval === true, "setID3v2() returned ".var_dump_string($retval)."\nin file: $tmpfile");
    $new_tags = $t->getID3v1();
    assert($new_tags !== false, "No tags found in file after setID3v1 w/ overwrite original tags = TRUE!");
    foreach($new_tags as $frameID => $value) {
        assert((string)$original_tags[$frameID] === $value, "overwrite original tags = TRUE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
    }
    unset($t,$retval);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = false
    $t = new TagLibMPEG($tmpfile);
    $original_tags = $t->getID3v1();
    $was_empty = ($original_tags === false);
    if($original_tags === false) {
        $original_tags = ['ALBUM' => '', 'ARTIST' => '', 'COMMENT' => ''];
    }
    $new_values = ['title'=>'testing'];
    $retval = $t->setID3v1($new_values, false);
    if(!$was_empty && isset($original_tags['title'])) {
        assert($retval === false, "overwrite original tags = FALSE\nsetID3v2() returned\n".var_dump_string($retval)."\noriginal tags were:\n".var_dump_string($original_tags)."\nin file: $tmpfile");
    } elseif(is_array($retval)) {
	foreach($retval as $frameID => $value) {
	    assert(isset($original_tags[$frameID]));
	}
    } else {
        assert($retval === true, "overwrite original tags = FALSE\nsetID3v2() returned\n".var_dump_string($retval)."\noriginal tags were:\n".var_dump_string($original_tags)."\nin file: $tmpfile");
    }
    $new_tags = $t->getID3v1();
    if($was_empty) {
        assert(isset($new_tags['TITLE']) && $new_tags['TITLE'] === 'testing', "New tags appeared found in file $tmpfile after setID3v1 w/ overwrite original tags = FALSE!\n".var_dump_string($new_tags));
    }
    if(is_array($new_tags)) {
        foreach($new_tags as $frameID => $value) {
	    if(isset($original_tags[$frameID])) {
	        assert($original_tags[$frameID] === $value, "overwrite original tags = FALSE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
	    } else {
		$k = strtolower($frameID);
		assert(isset($new_values[$k]) && $new_values[$k] === $value, "overwrite original tags = FALSE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
	    }
        }
    }
}
