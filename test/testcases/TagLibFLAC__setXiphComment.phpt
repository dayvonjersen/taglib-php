<?php
function Test__TagLibFLAC__setXiphComment($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = true
    $t = new TagLibFLAC($tmpfile);
    if($t->hasID3v1() || $t->hasID3v2()) {
        echo "\n\033[34mSkipping ",__FUNCTION__,"\033[0m for $tmpfile\n",
            "\033[34mKnown issue\033[0m: TagLib::FLAC::File::save() will keep any old ID3-tags up to date,\n",
            "if the file has no XiphComment, one will be constructed from the ID3 tags\n",
            "http://taglib.github.io/api/classTagLib_1_1FLAC_1_1File.html#aa478cd764bd1618ceef16c507528da65\n",
            "\033[34mKnown issue\033[0m: Cannot remove ID3 tags from FLAC files\n",
            "https://github.com/taglib/taglib/issues/646\n",
            "\033[1;34mTherefore\033[0m, cannot reliably test modifying XiphComment for file that has ID3v1 or ID3v2 tag.\n";
        return;
    }
    $original_tags = $t->getXiphComment();
    if($original_tags === false) {
        $original_tags = [];
    }
    $original_tags['TITLE'] = 'testing';
    $t->setXiphComment(['TITLE'=>'testing'], true);
    $new_tags = $t->getXiphComment();
    assert($new_tags !== false, "No tags found in file after setXiphComment w/ overwrite original tags = TRUE!");
    foreach($new_tags as $frameID => $value) {
        assert($original_tags[$frameID] == $value, "overwrite original tags = TRUE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
    }
    unset($t);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = false
    $t = new TagLibFLAC($tmpfile);
    $original_tags = $t->getXiphComment();
    if($original_tags === false) {
        $original_tags = [];
    }
    if(isset($original_tags['TITLE'])) {
	$original_tags['TITLE'] .= ' testing';
    } else {
	$original_tags['TITLE'] = 'testing';
    }
    $t->setXiphComment(['TITLE'=>'testing'], false);
    $new_tags = $t->getXiphComment();
    if(is_array($new_tags)) {
        foreach($new_tags as $frameID => $value) {
            assert($original_tags[$frameID] == $value, "overwrite original tags = FALSE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile\n".var_dump_string($original_tags));
        }
    }
    assert($new_tags !== false, "failed to set any tags\nin file: $tmpfile");
}
