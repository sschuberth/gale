<?php

if (!empty($file)) {
    header('Content-type: text/plain');
    header('Content-Disposition: attachment; filename="'.$file.'"');

    readfile(SERVER_TMP_DIRECTORY.$file);
    return;
}

require_once 'functions.php';

header('HTTP/1.0 404 Not Found');

?>

<?= '<?xml version="1.0" encoding="iso-8859-1"?>' ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">

<head>
    <title>glex - An OpenGL Extension Specification Parser</title>
    <link type="text/css" href="style.css" rel="stylesheet" />
</head>

<body>
    <div style="height: 128px"></div>

    <table style="width: 50%">
        <?php drawTableBorder('t','white'); ?>
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="text-align: center">
                <h2>Error 404: Not Found</h2>
                <h4>The requested file was not found on this server.</h4>
            </td>
            <td><!-- Corner spacer --></td>
        </tr>
        <?php drawTableBorder('b','white'); ?>
    </table>
</body>

</html>
