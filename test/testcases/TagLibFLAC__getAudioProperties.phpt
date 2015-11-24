<?php
function Test__TagLibFLAC__getAudioProperties($file, $result) {
    $t = new TagLibFLAC($file);
    $actual = $t->getAudioProperties();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
