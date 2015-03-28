<?php
$t = new TagLibMPEG("/home/tso/q/personal_jesus.mp3");
var_dump($t->setID3v1(
	['DATE' => '2005',
	 'ORIGINALDATE' => '2001'], true));

//var_dump($t->getID3v1());
