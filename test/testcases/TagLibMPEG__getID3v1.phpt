<?php
function Test__TagLibMPEG__getID3v1($file, $result) {
    $t = new TagLibMPEG($file);
    $actual = $t->getID3v1();
    if(is_array($actual)) {
        foreach($actual as $k => $v) {
            $cond = isset($result[$k]) && $result[$k] === $v;
            assert($cond, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
            if(!$cond) return;
        }
    } elseif(empty($actual)) {
        assert($actual === $result, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($actual)."\nFile: $file");
    }
}
