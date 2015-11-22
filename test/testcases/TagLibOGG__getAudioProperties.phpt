<?php
function Test__TagLibOGG__getAudioProperties($file, $result) {
    $t = new TagLibOGG($file);
    $actual = $t->getAudioProperties();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
