<?php
function Test__TagLibMPEG__setID3v2($file) {
    static $ranonce = false;
    __setID3v2_test1($file);
    __setID3v2_test2($file);
    __setID3v2_test3($file, $ranonce);
    __setID3v2_test4($file, $ranonce);
    $ranonce = true;
}

function __setID3v2_test1($file) {
    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    $t = new TagLibMPEG($tmpfile);
    $original_tags = $t->getID3v2();
    if($original_tags === false) {
        $original_tags = [];
    }
    $original_frames = [];
    foreach($original_tags as $tag) {
        assert(isset($tag['frameID']));
        $original_frames[] = $tag['frameID'];
    }
    $new_tags = [
        'TPE1' => 'testing'
    ];
    $t->setID3v2($new_tags);
    unset($t);

    $t = new TagLibMPEG($tmpfile);
    $tags = $t->getID3v2();
    assert(is_array($tags), "getID3v2 returned FALSE");
//var_dump($tags);
    foreach($tags as $tag) {
        assert(isset($tag['frameID'],$tag['data']));
        $frameID = $tag['frameID'];
        $data    = $tag['data'];
        if(isset($new_tags[$frameID])) {
            assert($tag['data'] === $new_tags[$frameID], "Expected: ".var_dump_string($new_tags[$frameID])."\nGot: ".var_dump_string($tag['data'])."\nIn file: $tmpfile");
        } elseif(in_array($frameID, $original_frames)) {
            $found = false;
            foreach($original_tags as $frame) {
                if($frame['frameID'] === $frameID && $frame['data'] === $data) {
                    $found = true;
                    break;
                }
            }
            assert($found, "ID3v2 tag altered.\nBefore:\n".var_dump_string($original_tags)."\nAfter:\n".var_dump_string($tags))."\nIn file: $tmpfile";
            if(!$found) break;
        } else {
            assert(false, "Unexpected frameID: $frameID\nBefore:\n".var_dump_string($original_tags)."\nNew Frames to Add:\n".var_dump_string($new_tags)."\nAfter:\n".var_dump_string($tags)."\nIn file: $tmpfile");
            break;
        }
    }
}

function __setID3v2_test2($file) {
    $t = new TagLibMPEG($file);
    $badvalues = [
    ['TBPM' => 120],
    "string",
    null,
    bool,
    12,
    (new stdClass),
    [0 => ['TPE1' => "value"]]
    ];
    foreach($badvalues as $badvalue) {
    $retval = @$t->setID3v2($badvalue);
        assert($retval === false, "sent non-typesafe value:\n".var_dump_string($badvalue)."\ngot back: \n".var_dump_string($retval)."\nin file: $tmpfile");
    }
}

function __setID3v2_test3($file, $ranonce) {
    // we don't need to do this test more than once tbh
    // Mon 23 Nov 2015 03:05:19 PM EST: this also strips any existing tags from the file first so it would be redundant in any case...
    // this test is designed to only test if all ID3v2 frames are supported.
    // except those which are explicitly NOT supported.
    if($ranonce) return;

    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    $good_future = [
        'APIC' => [
            'data' => base64_encode(file_get_contents('./testfiles/image.jpg')),
            'mime' => 'image/jpeg',
            'type' => TagLib::APIC_FRONTCOVER,
            'desc' => 'optional description'
        ],

        'COMM' => 0,
        'OWNE' => [
            'date' => @date('Ymd'), // php issues a stupid timezone warning literally no one cares php shut the fuck up
            'paid' => '$0.99',
            'seller' => 'WhyIsThisNotWorking:<'
        ],
        'PRIV' => [
            'owner' => 'nobody@example.com',
            'data' => base64_encode(bin2hex(openssl_random_pseudo_bytes(32)))
        ],
        'TALB' => 0,
        'TBPM' => 0,
        'TCOM' => 0,
        'TCON' => 0,
        'TCOP' => 0,
        'TDAT' => 0,
        'TDLY' => 0,
        'TENC' => 0,
        'TEXT' => 0,
        'TFLT' => 0,
        'TIME' => 0,
        'TIT1' => 0,
        'TIT2' => 0,
        'TIT3' => 0,
        'TKEY' => 0,
        'TLAN' => 0,
        'TLEN' => 0,
        'TMED' => 0,
        'TOAL' => 0,
        'TOFN' => 0,
        'TOLY' => 0,
        'TOPE' => 0,
        'TORY' => 0,
        'TOWN' => 0,
        'TPE1' => 0,
        'TPE2' => 0,
        'TPE3' => 0,
        'TPE4' => 0,
        'TPOS' => 0,
        'TPUB' => 0,
        'TRCK' => 0,
        'TRDA' => 0,
        'TRSN' => 0,
        'TRSO' => 0,
        'TSIZ' => 0,
        'TSRC' => 0,
        'TSSE' => 0,
        'TYER' => 0,
        'TXXX' => [
            'desc' => 'Example',
            'text' => 'http://www.example.com/'
        ],
        'UFID' => [
            'owner' => 'http://www.example.com/',
            'id' => sprintf("%d", time())
        ],
        'USLT' => 0,
        'WCOM' => 0,
        'WCOP' => 0,
        'WOAF' => 0,
        'WOAR' => 0,
        'WOAS' => 0,
        'WORS' => 0,
        'WPAY' => 0,
        'WPUB' => 0,
        'WXXX' => [
            'desc' => 'Example',
            'text' => 'http://www.example.com/'
        ]
    ];


    
    $t = new TagLibMPEG($tmpfile);
    $t->stripTags();
    foreach($good_future as $frame => $value) {
        if(!is_array($value)) {
            $value = strpos($frame,'W') === 0 ? 'http://www.example.com/' : 'test value';
            $good_future[$frame] = $value;
        }
        $retval = $t->setID3v2([$frame => $value]);
        assert($retval === true, "FAILED TO setID3v2(\n");//.var_dump_string([$frame => $value])."\n)\nin file: $tmpfile");
        if(!$retval) {
            // no point continuing if not in ASSERT_BAIL mode
            // will just throw a tonne of excess errors to output
            return;
        }
    }
    unset($t);
    $t = new TagLibMPEG($tmpfile);
    
    $res = $t->getID3v2();
    assert($res !== false, "no ID3v2 tag after setting\nin file: $tmpfile");

    foreach($res as $frame) {
        $frameID = $frame['frameID'];
        $actual = $frame['data'];
        assert(isset($good_future[$frameID]), "$frameID missing\nresult: ".var_dump_string($res)."\nin file: $tmpfile");
        $expected = $good_future[$frameID];

        if(is_string($expected)) {
            assert($actual === $expected, "$frameID differs\ntest value: ".
            var_dump_string($expected)."\nactual: ".var_dump_string($actual)."\nin file: $tmpfile");
        } else {
            switch($frameID) {
            case 'APIC':
                foreach($expected as $k => $v) {
                    assert(isset($frame[$k]), "Missing field $k in APIC:\n"
                    //.var_dump_string($actual)
                    ."\nin file: $tmpfile");
                    assert($v === $frame[$k], "APIC $k:\n"
                    //.var_dump_string($actual)."differs from expected:\n"
                    //.var_dump_string($expected[$v])
                    ."\nin file: $tmpfile");
                }
                break;
            case 'WXXX':
                $urlvalue = sprintf("[%s] %s", $expected['desc'], $expected['text']);
                assert($actual === $urlvalue, "Expected $frameID to have:\n".var_dump_string($urlvalue)
                ."\nactual: ".var_dump_string($actual)."\nin file: $tmpfile");
                break;
            case 'TXXX':
                $urlvalue = sprintf("[%s] %s %s", $expected['desc'], $expected['desc'], $expected['text']);
                assert($actual === $urlvalue, "Expected $frameID to have:\n".var_dump_string($urlvalue)
                ."\nactual: ".var_dump_string($actual)."\nin file: $tmpfile");
                break;
            case 'OWNE':
                $ownervalue = sprintf("pricePaid=%s datePurchased=%s seller=%s", $expected['paid'], $expected['date'], $expected['seller']);
                assert($actual === $ownervalue, "Expected $frameID to have:\n".var_dump_string($ownervalue)
                ."\nactual: ".var_dump_string($actual)."\nin file: $tmpfile");
                break;
            case 'PRIV':
                $privvalue = $expected['owner'];
                assert($actual === $privvalue, "Expected $frameID to have:\n".var_dump_string($privvalue)
                ."\nactual: ".var_dump_string($actual)."\nin file: $tmpfile");
                break;
            case 'UFID':
                assert($actual === '', "Expected $frameID to have\n".var_dump_string('')
                ."\nactual: ".var_dump_string($actual)."\nin file: $tmpfile");
                break;
            default:
                // xxx: temp:
                assert(false, "add test coverage for $frameID plx");
                echo $frameID,"\n";
                var_dump($actual);
                var_dump($expected);
            }
        }
    }

    $bad_future = ['AENC','COMR','ENCR','EQUA','ETCO','GEOB','GRID','IPLS','LINK','MCDI','MLLT','PCNT','POPM','POSS','RBUF','RVAD','RVRB','SYLT','SYTC','USER'];

    foreach($bad_future as $bad) {
        $res = @$t->setID3v2([$bad => 'shouldn\'t work']);
        assert($res === false, "$bad frame should NOT have been set but returned:\n ".var_dump_string($res)."\nin file: $tmpfile");
    }

    $res = $t->getID3v2();

    foreach($res as $frame) {
        $frameID = $frame['frameID'];
        assert(!in_array($frameID, $bad_future), "bad frame $frameID made it into file anyway! (silent failure?)\n\nor maybe it was in the file to begin with don't go chasing rainbows\nin file: $tmpfile");
    }
}

function __setID3v2_test4($file, $ranonce) {
    if($ranonce) return;

    $tmpfile = "./tmp/".basename($file);
    assert(copy($file, $tmpfile), "Couldn't copy $file to $tmpfile!");

    $t = new TagLibMPEG($tmpfile);
    $t->stripTags();

    $from_file = [
        'APIC' => [
            'file' => './testfiles/image.jpg',
            'mime' => 'image/jpeg',
            'type' => TagLib::APIC_FRONTCOVER,
            'desc' => 'optional description'
        ],
        'TIT2' => 'test'
    ];
    
    $res = $t->setID3v2($from_file);

    assert($res === true, "setID3v2 FAILED to set:\n"
        .var_dump_string($from_file)
        ."\nin file: $tmpfile");

    unset($t);
    $t = new TagLibMPEG($tmpfile);
    $res = $t->getID3v2();
    assert($res !== false, "no ID3v2 tag after setting\nin file: $tmpfile");

    assert(is_array($res) && isset($res[0]));
    $frame = $res[0];
    assert($frame['frameID'] === 'APIC');
    assert(isset($frame['data'],$frame['mime'],$frame['type'],$frame['desc']));
    $a = $from_file['APIC'];
    assert($frame['data'] === base64_encode(file_get_contents('./testfiles/image.jpg')));//$a['file'])));
    assert($frame['mime'] === $a['mime']);
    assert($frame['type'] === $a['type']);
    assert($frame['desc'] === $a['desc']);
}
