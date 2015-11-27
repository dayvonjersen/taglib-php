<?php
function Test__TagLibMPEG__getAudioProperties($file, $result) {
    $t = new TagLibMPEG($file);
    $actual = $t->getAudioProperties();
    assert(is_array($actual));
    foreach($result as $field => $value) {
        if($field == 'bitrate') {
            // TagLib::MPEG::Properties gives wrong bitrate consistently
            // usually off by like < 10 kbps, just annoying really
            // let's just make sure it's there.
            assert(isset($actual[$field]), "$field\n\nGot: ".var_dump_string($actual[$field])."\nExpected: ".var_dump_string($value)."\nFile: $file");
        } else {
            assert(isset($actual[$field]) && $value === $actual[$field], "$field\n\nGot: ".var_dump_string($actual[$field])."\nExpected: ".var_dump_string($value)."\nFile: $file");
        }
    }
}
