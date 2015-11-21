<?php
function Test__TagLibMPEG____construct($file) {
    $t = @new TagLibMPEG($file);
    assert($t instanceof TagLibMPEG);
    unset($t);
    $t = @new TagLibMPEG($file, true);
    assert($t instanceof TagLibMPEG);
    unset($t);
    $t = @new TagLibMPEG($file, 1);
    assert($t instanceof TagLibMPEG);
    unset($t);
    $t = @new TagLibMPEG($file, 1.0);
    assert($t instanceof TagLibMPEG);
    unset($t);
    $t = @new TagLibMPEG($file, []);
    assert($t instanceof TagLibMPEG);
    unset($t);
    $t = @new TagLibMPEG($file, "false");
    assert($t instanceof TagLibMPEG);
}
