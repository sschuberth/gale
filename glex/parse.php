<?php

require_once 'functions.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];
$argv=$_SERVER['argv'];

$cmdline=($argv[0]==$_SERVER['PHP_SELF'] || empty($_SERVER['PHP_SELF']));

if ($cmdline) {
    parse_str($argv[1]);
} else {
    // Suppress any errors in case these variables are not set.
    @$spec=$_REQUEST['spec'];
    @$debug=$_REQUEST['debug'];
}

if (empty($spec)) {
    if ($cmdline) {
        // When run from the command line, the spec has to be passed as an argument.
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file>');
    } else {
        // If the script is run on a web server, prompt for the spec.
        header('Location: index.php');
        return;
    }
}

// Parse the OpenGL specification into an associative array with section headers
// as keys and and section contents as values.
parseSpecIntoArray($spec,$struct);

if ($debug==1) {
    foreach (array_keys($struct) as $section) {
        // Dump only the section header.
        echo $section."\n";
    }
} else if ($debug==2) {
    // Dump the array containing the file structure if debugging is enabled.
    print_r($struct);
}

// Search for the section where new procedures and functions are defined.
foreach (array_keys($struct) as $section) {
    // We need to find strings like these (and variations):
    // "New Procedures and Functions"
    // "New Procedures, Functions and Structures:"
    if (preg_match('/New\W+Procedures\W+\w*\W*Functions/',$section)>0) {
        $content=$struct[$section];
        break;
    }
}

if (empty($content)) {
    $error='No valid content found';
    if ($cmdline) {
        // Print out the error message to the console.
        exit("Error: $error.");
    } else {
        // If the script is run on a web server, display an error on the index page.
        header('Location: index.php?spec='.$spec.'&error='.rawurlencode($error));
        return;
    }
    return;
}

// If this extension introduces new procedures and / or functions, create code
// to initialize them.
$extension=GLEX_PREFIX.$struct['Name'];

if ($cmdline)
    echo 'Writing macro header ...';
$p=writeMacroHeader($extension,$content);
if ($cmdline)
    echo ' saved as "'.$p."\".\n";

if ($cmdline)
    echo 'Writing prototype header ...';
$h=writePrototypeHeader($extension,$content);
if ($cmdline)
    echo ' saved as "'.$h."\".\n";

if ($cmdline)
    echo 'Writing initialization code ...';
$c=writeInitializationCode($extension);
if ($cmdline)
    echo ' saved as "'.$c."\".\n";

if (!$cmdline)
    require_once 'index.php';

?>
