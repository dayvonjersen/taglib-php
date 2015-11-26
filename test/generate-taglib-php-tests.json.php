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
        'filename' => $f
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
                $result = $file['id3v1'];
                break;
            case 'getID3v2':
                $result = $file['id3v2'];
                break;
            case 'getXiphComment':
                $result = $file['xiphcomment'];
                break;

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
        if(!is_null($q[1])) $id3v1['YEAR'] = $q[1];
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
        if(preg_match('/^([A-Z]{4}) \(.+\): (.*)/', $line, $m)) {
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
    assert(!empty(trim($text)), "oggz is either not installed or $file is not a valid OGG file");
    return trim($text);
}

function flacProperties($file) {
    return [
        'length' => (int)soxi($file, 'D'),
        'bitrate' => (int)preg_replace('/[^\d]/', '', soxi($file, 'B')),
        'sampleRate' => (int)soxi($file, 'r'),
        'channels' => (int)soxi($file, 'c'),
        'sampleWidth' => (int)soxi($file, 'b'),
        'sampleFrames' => (int)soxi($file, 's')
    ];
}

function mp3Properties($file) {
    $r = [
        'length' => (int)soxi($file, 'D'),
        'bitrate' => (int)preg_replace('/[^\d]/', '', soxi($file, 'B')),
        'sampleRate' => (int)soxi($file, 'r'),
        'channels' => (int)soxi($file, 'c'),

        //cheating a bit here
        'version' => 'MPEG Version 1',
        'protectionEnabled' => false,
        'isCopyrighted' => false,
        'isOriginal' => true
    ];
    // cheating a bit here
    $r['channelMode'] = $r['channels'] == 1 ? 'Mono': 'Stereo';
    return $r;
}

function oggProperties($file) {
    return [
        'length' => (int)soxi($file, 'D'),
        'bitrate' => (int)preg_replace('/[^\d]/', '', soxi($file, 'B')),
        'sampleRate' => (int)soxi($file, 'r'),
        'channels' => (int)soxi($file, 'c'),
        'vorbisVersion' => 0,

        // cheating a bit here
        'bitrateMinimum' => 0,
        'bitrateNominal' => 120000,
        'bitrateMaximum' => 0
    ];
}

echo json_encode($classes,JSON_PRETTY_PRINT);
