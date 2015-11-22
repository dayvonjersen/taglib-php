<?php
function Test__TagLibMPEG__setID3v1($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = true
    $t = new TagLibMPEG($tmpfile);
    $original_tags = $t->getID3v1();
    if($original_tags === false) {
        $original_tags = [];
    }
    $original_tags['TITLE'] = 'testing';
    $retval = $t->setID3v1(['TITLE'=>'testing'], true);
    assert($retval === true, "setID3v2() returned ".var_dump_string($retval));
    $new_tags = $t->getID3v1();
    assert($new_tags !== false, "No tags found in file after setID3v1 w/ overwrite original tags = TRUE!");
    foreach($new_tags as $frameID => $value) {
        assert($original_tags[$frameID] === $value, "overwrite original tags = TRUE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
    }
    unset($t,$retval);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = false
    $t = new TagLibMPEG($tmpfile);
    $original_tags = $t->getID3v1();
    $retval = $t->setID3v1(['TITLE'=>'testing'], false);
    assert($retval === true, "setID3v2() returned ".var_dump_string($retval));
    $new_tags = $t->getID3v1();
    if($original_tags === false) {
        assert($new_tags === false, "Tags were found in file $tmpfile after setID3v1 w/ overwrite original tags = FALSE!");
    }
    if(is_array($new_tags)) {
        foreach($new_tags as $frameID => $value) {
            assert($original_tags[$frameID] === $value, "overwrite original tags = FALSE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
        }
    }
}
