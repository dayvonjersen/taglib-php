<?php
function Test__TagLibFLAC__hasPicture($file) {
    $t = new TagLibFLAC($file);
    assert(is_bool($t->hasPicture()));
}
