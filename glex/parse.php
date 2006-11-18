<?php

require_once 'functions.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];
$argv=$_SERVER['argv'];

// Suppress any errors in case these variables are not set.
@$spec=$_REQUEST['spec'];
@$debug=$_REQUEST['debug'];

if (empty($spec)) {
    if ($argv[0]==$_SERVER['PHP_SELF']) {
        // When run from the command line, the spec has to be passed as an argument.
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file>');
    } else {
        // If the script is run on a web server, prompt for the spec.
        require_once 'form.php';
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

if (empty($content))
    return;

// If this extension introduces new procedures and / or functions, create code
// to initialize them.
$extension=$struct['Name'];
extractProcsToFile($extension,$content);
writePrototypeHeader($extension,$content);
writeInitializationCode($extension);

?>
