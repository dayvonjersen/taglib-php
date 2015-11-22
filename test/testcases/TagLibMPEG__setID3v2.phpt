<?php
function Test__TagLibMPEG__setID3v2($file) {
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
