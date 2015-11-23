<?php
function Test__TagLibOGGOGG__construct($file) {
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
    OGG__construct_test1($file);
    OGG__construct_test2($file,$badvalues);
}

function OGG__construct_test1($file) {
    try {
        $t = @new TagLibOGG($file);
        assert($t instanceof TagLibOGG);
    } catch(Exception $e) {
        assert(false, $e->getMessage());
    }
}


function OGG__construct_test2($file,$badvalues) {
    foreach($badvalues as $badvalue) {
	try {
	    $t = new TagLibOGG($badvalue);
	    assert($t === null, "sent non-typesafe value:\n".var_dump_string($badvalue)."\ngot back: \n".var_dump_string($t)."\nin file: $tmpfile");
	    $t = new TagLibOGG($file,$badvalue);
	    assert($t === null, "sent non-typesafe value:\n".var_dump_string($badvalue)."\ngot back: \n".var_dump_string($t)."\nin file: $tmpfile");
	} catch(Exception $e) {
	    assert($e instanceof Exception);
	    assert($t === null);
	} finally {
	    unset($t);
	}	
    }
}

