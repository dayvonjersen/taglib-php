<?php
function Test__TagLibOGG__stripTags($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file,$tmpfile), "Could not copy $file to $tmpfile!");

    $t = new TagLibOGG($tmpfile);
    $t->stripTags();
    unset($t);
    $t = new TagLibOGG($tmpfile);
    assert($t->hasXiphComment() === false, "$tmpfile has ID3v1 after calling stripTags!");
}
