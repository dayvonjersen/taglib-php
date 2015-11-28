<?php
function Test__TagLibFLAC__stripTags($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file,$tmpfile), "Could not copy $file to $tmpfile!");

    $t = new TagLibFLAC($tmpfile);
    if($t->hasID3v1() || $t->hasID3v2()) {
        echo "\n\033[34mSkipping ",__FUNCTION__, "\033[0m for $tmpfile\n",
            "\033[34mKnown issue\033[0m: Cannot remove ID3 tags from FLAC files\n",
            "https://github.com/taglib/taglib/issues/646\n";
        return;
    }
    $t->stripTags();
    unset($t);
    $t = new TagLibFLAC($tmpfile);
    assert($t->hasID3v1() === false, "$tmpfile has ID3v1 after calling stripTags!\n".var_dump_string($t->getID3v1()));
    assert($t->hasID3v2() === false, "$tmpfile has ID3v2 after calling stripTags!\n".var_dump_string($t->getID3v2()));
    assert($t->hasXiphComment() === false, "$tmpfile has XiphComment after calling stripTags!\n".var_dump_string($t->getXiphComment()));
}
