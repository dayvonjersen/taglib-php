<?php
function Test__TagLibMPEG__getID3v2($file, $result) {
    $t = new TagLibMPEG($file);
    $frames = $t->getID3v2();
    if(is_array($result) && is_array($frames)) {
        foreach($frames as $frame) {
            assert("isset(\$frame['frameID'],\$frame['data'])");
            $frameID = $frame['frameID'];
            $data    = $frame['data'];
            if($frameID === 'APIC') {
                assert("isset(\$frame['mime'],\$frame['type'],\$frame['desc'])");
                $mime = $frame['mime'];
                $type = $frame['type'];
                $desc = $frame['desc'];
                assert("\$mime === 'image/jpg'");
                assert("\$type === 3");
                assert("\$desc === ''");
                assert(base64_encode(file_get_contents('./testfiles/image.jpg')) === $data);
            } else {
                assert("isset(\$result['$frameID'])");
                assert($result[$frameID] === $data, 'Expected: '.var_dump_string($result[$frameID])."\nGot: ".var_dump_string($data)."\nFile: $file");
            }
        }
    } else {
        assert($result === $frames, 'Expected: '.var_dump_string($result)."\nGot: ".var_dump_string($frames)."\nFile: $file");
    }
}
