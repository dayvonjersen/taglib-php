<?php
function Test__TagLibFLAC__hasXiphComment($file, $result) {
    $t = new TagLibFLAC($file);
    $actual = $t->hasXiphComment();
    assert($actual === $result, 'Expected: ' . var_dump_string($result) . "\nGot: " . var_dump_string($actual) . "\nFile: $file");
}
