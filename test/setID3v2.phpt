<?php
$t = new TagLibMPEG('/home/tso/personal_jesus.mp3');
var_dump($t->setID3v2(['TOFN' => 'just give me a pain that I\'m used to']));
//var_dump($t->getID3v2());
