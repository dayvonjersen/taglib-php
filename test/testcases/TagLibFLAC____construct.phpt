<?php
function Test__TagLibFLAC____construct($file) {
    FLAC__construct_test1($file);
}

function FLAC__construct_test1($file) {
    try {
        $t = @new TagLibFLAC($file);
        assert($t instanceof TagLibFLAC);
    } catch (Exception $e) {
        assert(false, $e->getMessage());
    }
}

function FLAC__construct_test2($file) {
    $badvalues = [
        ['TBPM' => 120],
        "string",
        null,
        bool,
        12,
        (new stdClass),
        [0 => ['TPE1' => "value"]],
        "",
    ];
    foreach ($badvalues as $badvalue) {
        try {
            $t = new TagLibFLAC($badvalue);
            assert($t === null, "sent non-typesafe value:\n" . var_dump_string($badvalue) . "\ngot back: \n" . var_dump_string($t) . "\nin file: $tmpfile");
        } catch (Exception $e) {
            assert($e instanceof Exception);
            assert($t === null);
        } finally {
            unset($t);
        }
    }
}
