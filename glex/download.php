<?php

require_once 'constants.php';

if (empty($file)) {
    header('HTTP/1.0 404 Not Found');
    return;
}

header('Content-type: text/plain');
header('Content-Disposition: attachment; filename="'.$file.'"');

readfile(SERVER_TMP_DIRECTORY.$file);

?>
