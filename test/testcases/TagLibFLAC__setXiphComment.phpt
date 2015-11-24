<?php
function Test__TagLibFLAC__setXiphComment($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    // testing: overwrite original tags = true
    $t = new TagLibFLAC($tmpfile);
    $original_tags = $t->getXiphComment();
    if($original_tags === false) {
        $original_tags = [];
    }
    $original_tags['TITLE'] = 'testing';
    $t->setXiphComment(['TITLE'=>'testing'], true);
    $new_tags = $t->getXiphComment();
    assert($new_tags !== false, "No tags found in file after setXiphComment w/ overwrite original tags = TRUE!");
    foreach($new_tags as $frameID => $value) {
        assert($original_tags[$frameID] === $value, "overwrite original tags = TRUE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile");
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
            assert($original_tags[$frameID] === $value, "overwrite original tags = FALSE\n$frameID was:\n".var_dump_string($original_tags[$frameID])."\nis now:\n".var_dump_string($value)."\nin file: $tmpfile\n".var_dump_string($original_tags));
        }
    }
    assert($new_tags !== false, "failed to set any tags\nin file: $tmpfile");
}
