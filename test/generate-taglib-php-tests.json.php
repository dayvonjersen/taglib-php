<?php
assert_options(ASSERT_BAIL, 1);
$classes = [];
$ext = new ReflectionExtension('taglib');
foreach($ext->getClasses() as $class) {
    $className = $class->getName();
    $methods = [];
    foreach($class->getMethods() as $method) {
        if($method->isPublic() && !$method->isStatic()) {
            $methods[$method->getName()] = [];
        }
    }
    $classes[$className] = $methods;
}

assert(chdir('./testfiles'));
$d = dir('.');
$flacs = $mp3s = $oggs = [];
while(($f = $d->read())!==false) {
    if(is_dir($f)) continue;

    $info = [
        'filename' => "./testfiles/$f"
    ];

    switch(end(explode('.', $f))) {
    case 'flac':
        $info['properties'] = flacProperties($f);
        $info['xiphcomment'] = metaflac($f);
        $info['id3v1'] = id3v1($f);
        $info['id3v2'] = id3v2($f);

        $flacs[] = $info;
        break;
    case 'mp3':
        $info['properties'] = mp3Properties($f);
        $info['id3v1'] = id3v1($f);
        $info['id3v2'] = id3v2($f);

        $mp3s[] = $info;
        break;
    case 'ogg':
        $info['properties'] = oggProperties($f);
        $info['xiphcomment'] = oggz($f);

        $oggs[] = $info;
        break;
    }

    unset($info);
}

foreach($classes as $class => $methods) {
    switch($class) {
    case 'TagLib':
        continue 2;
    
    case 'TagLibMPEG':
        $testfiles = $mp3s;
        break;
    case 'TagLibFLAC':
        $testfiles = $flacs;
        break;
    case 'TagLibOGG':
        $testfiles = $oggs;
        break;
    default:
        echo 'Unrecognized class ', $class, "\n";
        exit(1);
    }
    foreach($methods as $method => $array) {

        foreach($testfiles as $file) {
            $params = [$file['filename']];

            switch($method) {
            case 'getAudioProperties':
                $result = $file['properties'];
                break;
            case 'hasID3v1':
                $result = !empty($file['id3v1']);
                break;
            case 'hasID3v2':
                $result = !empty($file['id3v2']);
                break;
            case 'hasXiphComment':
                $result = !empty($file['xiphcomment']);
                break;
            case 'getID3v1':
                $result = empty($file['id3v1']) ? false : $file['id3v1'];
                break;
            case 'getID3v2':
                $result = empty($file['id3v2']) ? false : $file['id3v2'];
                break;
            case 'getXiphComment':
                $result = $file['xiphcomment'];
                $result = empty($file['xiphcomment']) ? false : $file['xiphcomment'];
                break;

            case 'hasPicture':
            case 'getPictures':
            case 'setPicture':
            case '__construct':
            case 'setID3v1':
            case 'setID3v2':
            case 'setXiphComment':
            case 'stripTags':
                $result = null;
                break;
            default:
                echo 'Unknown method ', $method, "\n";
                exit(1);
            }

            $classes[$class][$method][] = [
                'params' => $params,
                'result' => $result
            ];
            unset($params, $result);
        }
    }
}

function metaflac($file) {
    $text = `metaflac --list $file`;
    assert(!empty(trim($text)), "metaflac is either not installed or $file is not a FLAC");
    $xiph = [];
    foreach(explode("\n", $text) as $line) {
        $line = ltrim($line);
        if(preg_match('/comment\[\d+\]: (.+)/', $line, $m)) {
            list($frame,$value) = explode('=', $m[1]);
            $xiph[$frame] = $value;
        }
    }
    return $xiph;
}

function id3v1($file) {
    $text = `id3v2 -l $file`;
    assert(!empty(trim($text)), "id3v2 is either not installed or $file is not a valid audio file");
    
    if(preg_match("/".preg_quote($file).": No ID3(v1)? tag/m", $text)) {
        return [];
    }

    if(preg_match("/id3v1 tag info for ".preg_quote($file).":\n(.*)(id3v2 tag info for $file)?/ms", $text, $m)) {
        $text = $m[1];
        $id3v1 = [];
        preg_match("/Year: (\d+), Genre: (.*) \(\d+\)/", $text, $q);
        if(!is_null($q[1])) $id3v1['DATE'] = $q[1];
        if(!is_null($q[2])) $id3v1['GENRE'] = $q[2];
        $text = str_replace($q[0], '', $text);
        preg_match_all("/(Title|Artist|Album|Comment|Track)\s*: ([^\s]{0,30})/", $text, $n, PREG_SET_ORDER);
        foreach($n as $o) {
            if(!rtrim($o[2])) continue;
            if($o[1] == 'Track') $o[1] = 'TRACKNUMBER';
            $id3v1[strtoupper($o[1])] = rtrim($o[2]);
        }
        return $id3v1;
    }
    echo $text;
    assert(false, "Something happened.");
}

function id3v2($file) {
    $text = `id3v2 -l $file`;
    assert(!empty(trim($text)), "id3v2 is either not installed or $file is not a valid audio file");
    
    if(preg_match("/^$file: No ID3v2 tag$/m", $text)) {
        return [];
    }

    $id3v2 = [];
    foreach(explode("\n", $text) as $line) {
        if(preg_match('/^([A-Z0-9]{4}) \(.+\): (.*)/s', $line, $m)) {
            $frame = $m[1];
            $value = $m[2];
            switch($frame) {
            case 'APIC':
                // cheating a bit here
                $value = '...';
                break;
            case 'COMM':
                $value = preg_replace('/^\(\)\[.*\]: /', '', $value);
                break;
            case 'TCON':
                $value = preg_replace('/^.*\((\d+)\).*/', '\1', $value);
                break;
            case 'WXXX':
            case 'TXXX':
                $value = preg_replace('/^\((.*?)\): /', '[\1] ', $value);
                if(!strstr($value, '[')) $value = '[] '.$value;
                break;
            case 'TYER':
                $frame = 'TDRC';
                break;
            }
            $id3v2[$frame] = $value;
        }
    }
    return $id3v2;
}

function oggz($file) {
    $text = `oggz comment -l $file`;
    assert(!empty(trim($text)), "oggz is either not installed or $file is not a valid OGG file");

    $xiph = [];
    foreach(explode("\n", $text) as $line) {
        $line = ltrim($line);
        if(preg_match('/^([A-Z]+): (.*)/', $line, $m)) {
            $xiph[$m[1]] = $m[2];
        }
    }
    return $xiph;
}

function soxi($file, $flag) {
    $text = `soxi -$flag $file`;
    assert(!empty(trim($text)), "soxi is either not installed or $file is not a valid audio file");
    return trim($text);
}

function flacProperties($file) {
    $p = [
        'length' => (int)soxi($file, 'D'),
        'sampleRate' => (int)soxi($file, 'r'),
        'channels' => (int)soxi($file, 'c'),
        'sampleWidth' => (int)soxi($file, 'b'),
        'sampleFrames' => (int)soxi($file, 's')
    ];
    if($p['length'] > 0) {
        $tmpfile = "tmpxxx___$file";
        assert(copy($file, $tmpfile), "failed to copy file");
        `metaflac --remove-all --dont-use-padding $tmpfile`;
        `id3v2 -D $tmpfile`;
        $text = `metaflac --list $tmpfile`;
        assert(preg_match('/length: (\d+)/', $text, $b));
        $streamLength = filesize($tmpfile) - $b[1];
        unlink($tmpfile);
        $p['bitrate'] = (int)( $streamLength * 8 / $p['length'] / 1000 );
    } else {
        $p['bitrate'] = 0;
    }
    return $p;
}

function mp3Properties($file) {
    $text = `mp3info -x $file`;
    assert(!empty(trim($text)), "mp3info is either not installed for $file is not a valid MP3.");
    $p = [];
    foreach(explode("\n", $text) as $line) {
        if(preg_match("/^Media Type:  MPEG ([\d\.]+) Layer (I{1,3})/", $line, $m)) {
            $v = (float)$m[1];
            if((int)$v == $v) $v = (int)$v;
            $p['version'] = "MPEG Version $v";
            $p['layer'] = strlen($m[2]);
            continue;
        }
        if(preg_match("/^Audio:\s+(Variable|\d+) kbps, (\d+) kHz \(([\w\s]+)\)/", $line, $m)) {
            if($m[1] === 'Variable') {
                $text2 = `mp3info -r a -x $file`;
                foreach(explode("\n", $text2) as $line2) {
                    if(preg_match("/^Audio:\s+([\d\.]+) kbps/", $line2, $m2)) {
                        $p['bitrate'] = (int)$m2[1];
                        break;
                    }
                }
            } else {
                $p['bitrate'] = (int)$m[1];
            }
            $khz = $m[2];
            $hz = 0;
            switch($khz) {
            case 11: $hz = 11025; break;
            case 22: $hz = 22050; break;
            case 44: $hz = 44100; break;
            case 88: $hz = 88200; break;
            case 176: $hz = 176400; break;
            case 352: $hz = 352800; break;
            default: $hz = $khz * 1000;
            }
            $p['sampleRate'] = $hz;
            $mode = $m[3];
            $chan = 0;
            switch($mode) {
            case 'mono':
                $mode = 'Mono';
                $chan = 1;
                break;
            case 'stereo':
                $mode = 'Stereo';
                $chan = 2;
                break;
            case 'dual mono':
                $mode = 'Dual Mono';
                $chan = 2;
                break;
            }
            $p['channelMode'] = $mode;
            $p['channels'] = $chan;
            continue;
        }
        if(preg_match("/^(Copyright|Original):\s+(Yes|No)/", $line, $m)) {
            if($m[1] == 'Copyright') {
                $m[1].= 'ed';
            }
            $p["is{$m[1]}"] = ($m[2] == 'Yes');
            continue;
        }
        if(preg_match("/^Length:\s+([\d:]+)/", $line, $m)) {
            $t = explode(':', $m[1]);
            $h=$m=$s=0;
            switch(count($t)) {
            case 3:
                $h=(int)$t[0];
                $m=(int)$t[1];
                $s=(int)$t[2];
                break;
            case 2:
                $m=(int)$t[0];
                $s=(int)$t[1];
                break;
            default: assert(false, "$file\n\n$line");
            }
            $p['length'] = $h*3600+$m*60+$s;
            continue;
        }
    }

    //cheating a bit here
    $p['protectionEnabled'] = false;

    return $p;
}

function oggProperties($file) {
    $p = [
        'length' => (int)soxi($file, 'D'),
        'sampleRate' => (int)soxi($file, 'r'),
        'channels' => (int)soxi($file, 'c'),
        'vorbisVersion' => 0
    ];

    $text = `ogginfo $file`;
    assert(!empty(trim($text)), "ogginfo is either not installed or $file is not a valid OGG.");
    foreach(explode("\n", $text) as $line) {
        if(preg_match("/^Nominal bitrate: ([\d\.]+)/", $line, $m)) {
            $p['bitrateNominal'] = ((int)$m[1])*1000;
            continue;
        }
        if(preg_match("/^(Upper|Lower) bitrate/", $line, $m)) {
            $key = $m[1] == 'Upper' ? 'Maximum' : 'Minimum';
            if(strstr($line, 'not set')) {
                $value = 0;
            } elseif(preg_match("/([\d\.]+)/", $line, $m2)) {
                $value = ((int)$m2[1])*1000;
            } else {
                assert(false, "$file\n\n$line");
            }
            $p["bitrate$key"] = $value;
            continue;
        }
    }

    $p['bitrate'] = isset($p['bitrateNominal']) ? (int)($p['bitrateNominal']/1000) : 0;
    return $p;
}

echo json_encode($classes,JSON_PRETTY_PRINT);
