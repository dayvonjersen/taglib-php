<?php
function Test__TagLibFLAC__getPictures($file) {
    $t = new TagLibFLAC($file);
    $res = $t->getPictures();
    if($t->hasPicture()) {
        assert(is_array($res));
    } else {
        assert($res === []);
    }
}
