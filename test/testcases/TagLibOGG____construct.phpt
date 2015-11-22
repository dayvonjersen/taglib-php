<?php
function Test__TagLibOGG____construct($file) {
    $t = @new TagLibOGG($file);
    assert($t instanceof TagLibOGG);
    unset($t);
    $t = @new TagLibOGG($file, true);
    assert($t instanceof TagLibOGG);
    unset($t);
    $t = @new TagLibOGG($file, 1);
    assert($t instanceof TagLibOGG);
    unset($t);
    $t = @new TagLibOGG($file, 1.0);
    assert($t instanceof TagLibOGG);
    unset($t);
    $t = @new TagLibOGG($file, []);
    assert($t instanceof TagLibOGG);
    unset($t);
    $t = @new TagLibOGG($file, "false");
    assert($t instanceof TagLibOGG);
}
