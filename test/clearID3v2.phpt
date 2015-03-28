<?php
$t = new TagLibMPEG('/home/tso/personal_jesus.mp3');
$t->clearID3v2();
echo print_r($t->getID3v2());
