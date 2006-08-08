<?php

require 'constants.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];

if (isset($argc)) {
    $argv=$_SERVER['argv'];

    if ($argc<2)
        exit('Usage: '.basename($argv[0]).' spec=<URL to OpenGL extension specification text file>');

    // Make the command line available as PHP variables.
    for ($i=0;$i<$argc;++$i) {
        parse_str($argv[$i],$tmp);
        $_REQUEST=array_merge($_REQUEST,$tmp);
    }
}

$spec=$_REQUEST['spec'];

if (empty($spec))
    exit('Error: The given "spec" argument is empty.');

$handle=@fopen($spec,'r') or
    exit("Error: Unable to open the file \"$spec\" for reading.");

// Read the all needed parts of the file into a buffer.
$i=0;
while (!feof($handle)) {
    $line=fgets($handle);
    ++$i;
    if (strlen($line)>OPENGL_SPEC_MAX_CHARS_PER_LINE)
        echo "Warning: Line $i exceeds the maximum of ".OPENGL_SPEC_MAX_CHARS_PER_LINE." characters.\n";
    if (is_int(strpos($line,OPENGL_SPEC_BREAK_STRING)))
        break;
    $buffer.=$line;
}

echo $buffer;

fclose($handle);

?>
