<?php
function Test__TagLibMPEG__stripTags($file) {
    $tmpfile = "./tmp/" . basename($file);
    assert(copy($file, $tmpfile), "Could not copy $file to $tmpfile!");

    $t = new TagLibMPEG($tmpfile);
    $t->stripTags();
    unset($t);
    $t = new TagLibMPEG($tmpfile);
    assert($t->hasID3v1() === false, "$tmpfile has ID3v1 after calling stripTags!");
    assert($t->hasID3v2() === false, "$tmpfile has ID3v2 after calling stripTags!");
}
