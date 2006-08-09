<?php

require 'constants.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];

if (isset($argc)) {
    $argv=$_SERVER['argv'];

    if ($argc<2)
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file>');

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

// Parse the sections and contents of the file into a structure array.
$section=NULL;
$content=NULL;

$i=0;
while (!feof($handle)) {
    $line=rtrim(fgets($handle));
    ++$i;

    if (strlen($line)>OPENGL_SPEC_MAX_CHARS_PER_LINE)
        echo "Warning: Line $i exceeds the maximum of ".OPENGL_SPEC_MAX_CHARS_PER_LINE." characters.\n";

    // We can stop parsing when we have reached e.g. the revision history.
    if (is_int(strpos($line,OPENGL_SPEC_BREAK_STRING)))
        break;

    if (empty($line) || $line[0]==' ') {
        // Switch to content parsing mode until a new section header is found.
        if (!empty($content))
            $content.=' ';
        $content.=ltrim($line);
    } else {
        // Skip comments as shown in the OpenGL extension document template.
        if ($line[0]=='*')
            continue;

        // Add any new section and (possibly empty) content to the structure array.
        if (!is_null($section) && !is_null($content)) {
            $struct[$section]=$content;
            $section=NULL;
            $content=NULL;
        }

        // Switch to section parsing mode until indented text in found.
        if (!empty($section))
            $section.=' ';
        $section.=$line;
    }
}
$struct[$section]=$content;

//print_r($struct);

fclose($handle);

?>
