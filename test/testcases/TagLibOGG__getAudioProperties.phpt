<?php
function Test__TagLibOGG__getAudioProperties($file, $result) {
    $t = new TagLibOGG($file);
    $actual = $t->getAudioProperties();
    foreach($actual as $field => $value) {
        assert(isset($result[$field]) && $value === $result[$field], "$field\n\nExpected: ".var_dump_string($result[$field])."\nGot: ".var_dump_string($value)."\nFile: $file");
    }
}
