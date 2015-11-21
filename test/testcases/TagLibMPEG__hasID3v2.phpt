<?php
function Test__TagLibMPEG__hasID3v2($file, $result) {
    $t = new TagLibMPEG($file);
    $actual = $t->hasID3v2();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
