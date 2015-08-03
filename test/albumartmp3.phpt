<?php
$t = new TagLibMPEG('/home/tso/personal_jesus.mp3');

$t->setID3v2(['APIC' => ['data' => base64_encode(file_get_contents('/home/tso/test.jpg')),
'mime' => 'text/jpeg',
'type' => TagLib::APIC_COLOUREDFISH,
'desc' => 'testerino']]);

echo 'set picture';
$u = $t->getID3v2();


var_dump($u);

file_put_contents('/home/tso/output.jpeg', base64_decode($u['APIC']['data']));

