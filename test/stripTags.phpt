<?php
$t = new TagLibMPEG('/home/tso/personal_jesus.mp3');
$t->stripTags(TagLibMPEG::STRIP_ID3V2);
echo print_r($t->getID3v2());
