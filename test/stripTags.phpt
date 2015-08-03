<?php
error_reporting(E_ALL);
$t = new TagLibMPEG('/home/tso/personal_jesus.mp3');
if($t->stripTags()) {
    echo print_r($t->getID3v2());
}
