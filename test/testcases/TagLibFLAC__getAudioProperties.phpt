<?php
function Test__TagLibFLAC__getAudioProperties($file, $result) {
    $t = new TagLibFLAC($file);
    $actual = $t->getAudioProperties();
    assert(is_array($actual));
    foreach($result as $field => $value) {
        if($field == 'bitrate') {
            // allow for rounding errors
            assert(abs($actual[$field] - $value) < 2, "$field\n\nGot: ".var_dump_string($actual[$field])."\nExpected: ".var_dump_string($value)."\nFile: $file");
        } else {
            assert(isset($actual[$field]) && $value === $actual[$field], "$field\n\nGot: ".var_dump_string($actual[$field])."\nExpected: ".var_dump_string($value)."\nFile: $file");
        }
    }
}
