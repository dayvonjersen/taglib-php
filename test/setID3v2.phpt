<?php
$t = new TagLibMPEG('/home/tso/personal_jesus.mp3');
var_dump($t->setID3v2(['TXXX' => 'just a test']));
var_dump($t->getID3v2());
