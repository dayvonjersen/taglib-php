<?php
$pass_counter = $fail_counter = 0;
assert_options(ASSERT_ACTIVE, 1);
assert_options(ASSERT_WARNING, 0);
assert_options(ASSERT_BAIL, 0);
assert_options(ASSERT_QUIET_EVAL, 1);
assert_options(ASSERT_CALLBACK, function($file,$line,$code,$desc="") {
    global $fail_counter;
    $fail_counter++;
    echo "\n---\n\033[31mFAIL:\033[0m",basename($file), ":$line\n$code\n$desc\n---\n";
});

assert(extension_loaded('taglib'), 'TagLib extension not loaded!');

function var_dump_string($var) {
    ob_start();
    var_dump($var);
    $ret = ob_get_contents();
    ob_end_clean();
    return $ret;
}

$d = dir('./testcases');
while(($f = $d->read())!==false) {
    if(!is_dir($f) && preg_match('/.phpt$/', $f)) {
        require_once './testcases/'.basename($f);
    }
}

// use generate-taglib-php-tests.json.php to generate this file,
// then edit manually
$JSON = json_decode(file_get_contents('taglib-php-tests.json'),1);
/*
{
    "TagLibMPEG": {
        "getAudioProperties": [
            {
                "params": ["empty.mp3"],
                "result": {"length": "1"}//...
            },//...
        ],//...
    },//...
*/
$num_tests = 0;
foreach($JSON as $class => $methods) {
    foreach($methods as $method => $tests) {
        $testFn = "Test__{$class}__{$method}";
        if(!function_exists($testFn)) {
            echo "\033[33mWarning: no test coverage for $class::$method!\033[0m\n";
        } else {
            $count = count($tests);
            $num_tests += $count;
            $preprefail = $fail_counter;
            echo "Running $count tests for $class::$method";
            foreach($tests as $test) {
                $args = $test['params'];
                $args[] = $test['result'];
                $prefail = $fail_counter;
                call_user_func_array($testFn,$args);
                if($prefail === $fail_counter) {
                    $pass_counter++;
                    if($prefail === $preprefail) {
                        echo ".";
                    }
                }
            }
            if($preprefail === $fail_counter) {
                echo "\033[32mOK\033[0m\n";
            }
        }
    }
}
echo "\n\n$num_tests tests\n$pass_counter PASS\n\n$fail_counter FAIL(ed assertions)\n";
