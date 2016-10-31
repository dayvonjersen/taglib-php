<?php
function Test__TagLib__getPictureTypeAsString() {
    $expected = [
        0  => 'Other',
        1  => 'File Icon',
        2  => 'Other File Icon',
        3  => 'Front Cover',
        4  => 'Back Cover',
        5  => 'Leaflet Page',
        6  => 'Media',
        7  => 'Lead Artist',
        8  => 'Artist',
        9  => 'Conductor',
        10 => 'Band',
        11 => 'Composer',
        12 => 'Lyricist',
        13 => 'Recording Location',
        14 => 'During Recording',
        15 => 'During Performance',
        16 => 'Movie Screencapture',
        17 => '<°))))><',
        18 => 'Illustration',
        19 => 'Band Logo',
        20 => 'Publisher Logo',
    ];
    foreach($expected as $type => $value) {
        $actual = TagLib::getPictureTypeAsString($type);
        assert($actual === $value, sprintf('type: %s expected: %s got: %s', var_dump_string($type), var_dump_string($value), var_dump_string($actual)));
    }
    $bogus = [
        false,
        null,
        -1,
        0.4343443443,
        new stdClass,
        []
    ];
    $value = false;
    foreach($bogus as $type) {
        $actual = TagLib::getPictureTypeAsString($type);
        assert($actual === $value, sprintf('type: %s expected: %s got: %s', var_dump_string($type), var_dump_string($value), var_dump_string($actual)));
    }
}
