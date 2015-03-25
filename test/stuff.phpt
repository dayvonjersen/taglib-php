<?php
setlocale(LC_ALL, "en_US.UTF-8");
$t = new TagLibMPEG("/home/tso/personal_jesus.mp3");
var_dump($t->getAudioProperties());
var_dump($t->getID3v2());
