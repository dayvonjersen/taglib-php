<?php
function Test__TagLibMPEG__hasAPE($file, $result) {
    $t = new TagLibMPEG($file);
    $actual = $t->hasAPE();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
