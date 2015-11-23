<?php
function Test__TagLibMPEG____construct($file) {
    $badvalues = [
	['TBPM' => 120],
	"string",
	null,
	bool,
	12,
	(new stdClass),
	[0 => ['TPE1' => "value"]],
	""
    ];
    ____construct_test1($file);
    ____construct_test2($file,$badvalues);
}

function ____construct_test1($file) {
    try {
        $t = @new TagLibMPEG($file);
        assert($t instanceof TagLibMPEG);
    } catch(Exception $e) {
        assert(false, $e->getMessage());
    }
}


function ____construct_test2($file,$badvalues) {
    foreach($badvalues as $badvalue) {
	try {
	    $t = new TagLibMPEG($badvalue);
	    assert($t === null, "sent non-typesafe value:\n".var_dump_string($badvalue)."\ngot back: \n".var_dump_string($t)."\nin file: $tmpfile");
	    $t = new TagLibMPEG($file,$badvalue);
	    assert($t === null, "sent non-typesafe value:\n".var_dump_string($badvalue)."\ngot back: \n".var_dump_string($t)."\nin file: $tmpfile");
	} catch(Exception $e) {
	    assert($e instanceof Exception);
	    assert($t === null);
	} finally {
	    unset($t);
	}	
    }
}