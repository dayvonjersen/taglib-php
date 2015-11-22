<?php
function Test__TagLibOGG__getXiphComment($file, $result) {
    $t = new TagLibOGG($file);
    $actual = $t->getXiphComment();
    if(isset($result['METADATA_BLOCK_PICTURE'])) {
/**
 * this perl script isn't what I set the tag with and as a result
 * gives false negatives
 */
 /*
$tmp = '/tmp/oggflacblock-_'.bin2hex(openssl_random_pseudo_bytes(32));
$out = '/tmp/oggflacblock_fail'.bin2hex(openssl_random_pseudo_bytes(32));
$abs = realpath('./testfiles/image.jpg');
$exit = exec("./oggflacblock.pl -i $abs -o $tmp -write block64 >$out 2>$out");
$out = trim(file_get_contents($out));
if(strlen($out)) {
    echo $out;
    exit(1);
}
assert(file_exists($tmp));
$result['METADATA_BLOCK_PICTURE'] = trim(file_get_contents($tmp));
unlink($tmp);

if($actual['METADATA_BLOCK_PICTURE'] != $result['METADATA_BLOCK_PICTURE']) {
    file_put_contents('/tmp/expected',  $result['METADATA_BLOCK_PICTURE']);
    file_put_contents('/tmp/actual',  $actual['METADATA_BLOCK_PICTURE']);
    exit;
}
*/

        $result['METADATA_BLOCK_PICTURE'] = trim(file_get_contents('./testfiles/metadata_block_picture'));
    }
    foreach($actual as $frameID => $data) {
        assert($data === $result[$frameID], 'Expected: '.var_dump_string($result[$frameID])."\nGot: ".var_dump_string($data)."\nFile: $file");
    }
}
