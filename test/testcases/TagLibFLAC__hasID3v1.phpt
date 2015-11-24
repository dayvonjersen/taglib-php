<?php
function Test__TagLibFLAC__hasID3v1($file, $result) {
    $t = new TagLibFLAC($file);
    $actual = $t->hasID3v1();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
