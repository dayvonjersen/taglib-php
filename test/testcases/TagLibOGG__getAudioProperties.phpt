<?php
function Test__TagLibOGG__getAudioProperties($file, $result) {
    $t = new TagLibOGG($file);
    $actual = $t->getAudioProperties();
    foreach ($result as $field => $value) {
        assert(isset($actual[$field]) && $value === $actual[$field], "$field\n\nGot: " . var_dump_string($actual[$field]) . "\nExpected: " . var_dump_string($value) . "\nFile: $file");
    }
}
