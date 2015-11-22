<?php
function Test__TagLibOGG__hasXiphComment($file, $result) {
    $t = new TagLibOGG($file);
    $actual = $t->hasXiphComment();
    assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
}
