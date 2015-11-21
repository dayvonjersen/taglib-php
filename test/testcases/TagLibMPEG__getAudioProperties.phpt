<?php
function Test__TagLibMPEG__getAudioProperties($file, $result) {
    $t = new TagLibMPEG($file);
    $actual = $t->getAudioProperties();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
