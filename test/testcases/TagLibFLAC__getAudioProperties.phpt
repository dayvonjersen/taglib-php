<?php
function Test__TagLibFLAC__getAudioProperties($file, $result) {
    $t = new TagLibFLAC($file);
    $actual = $t->getAudioProperties();
    assert(is_array($actual));
    foreach($actual as $field => $value) {
        assert(isset($result[$field]) && $value === $result[$field], "$field\n\nExpected: ".var_dump_string($result[$field])."\nGot: ".var_dump_string($value)."\nFile: $file");
    }
}
