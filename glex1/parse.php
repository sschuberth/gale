<?php

require_once 'functions.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];
$argv=$_SERVER['argv'];

$cmdline=($argv[0]==$_SERVER['PHP_SELF'] || empty($_SERVER['PHP_SELF']));

if ($cmdline) {
    for ($i=1;$i<$argc;++$i) {
        parse_str($argv[$i]);
    }
}
else {
    // Suppress any errors in case these variables are not set.
    @$spec=$_REQUEST['spec'];
    @$debug=$_REQUEST['debug'];
}

if (empty($spec)) {
    if ($cmdline) {
        // When run from the command line, the spec has to be passed as an argument.
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file> [debug=<verbosity level 1 or 2>]');
    }
    else {
        // If the script is run on a web server, prompt for the spec.
        header('Location: index.php');
        return;
    }
}

// Parse the OpenGL specification into an associative array with section headers
// as keys and and section contents as values.
parseSpecIntoArray($spec,$struct);

if ($debug==1) {
    // Dump the array containing the file structure if debugging is enabled.
    echo "--- BEGIN debug output ---\n";
    print_r($struct);
    echo "--- END debug output ---\n";
}

// Search for the section where new procedures and functions are defined.
$remaining=2;
foreach (array_keys($struct) as $section) {
    // We need to find strings like these (and variations):
    // "New Procedures and Functions"
    // "New Procedures, Functions and Structures:"
    if (preg_match('/New\W+Procedures\W+\w*\W*Functions/',$section)>0) {
        $procs=$struct[$section];
        --$remaining;
    }

    if (preg_match('/New\W+Tokens/',$section)>0) {
        $tokens=$struct[$section];
        --$remaining;
    }

    if ($remaining==0) {
        break;
    }
}

if (empty($procs)) {
    $error='No valid content found';
    if ($cmdline) {
        // Print out the error message to the console.
        exit("Error: $error.");
    }
    else {
        // If the script is run on a web server, display an error on the index page.
        header('Location: index.php?spec='.$spec.'&error='.rawurlencode($error));
    }
    return;
}

// If this extension introduces new procedures and / or functions, create code
// to initialize them.
$extension=GLEX_PREFIX.$struct['Name'];

// In case of multiple extension names (like WGL_ARB_create_context / WGL_ARB_create_context_profile)
// use the first one.
$extension=strtok($extension,' ');

if ($cmdline) {
    echo 'Writing macro header ... ';
    $p=writeMacroHeader($extension,$procs);
    if (empty($p)) {
        echo "skipped empty file.\n";
    }
    else {
        echo 'saved as "'.$p."\".\n";
    }

    echo 'Writing prototype header ... ';
    $h=writePrototypeHeader($extension,$procs,$tokens);
    if (empty($h)) {
        echo "skipped empty file.\n";
    }
    else {
        echo 'saved as "'.$h."\".\n";
    }

    echo 'Writing initialization code ... ';
    $c=writeInitializationCode($extension);
    if (empty($c)) {
        echo "skipped empty file.\n";
    }
    else {
        echo 'saved as "'.$c."\".\n";
    }

    $g='GLEX_globals.h';
    if (file_exists($g)===FALSE) {
        echo 'Copying globals header ...';
        copy(dirname($argv[0]).'/'.$g,$g);
        echo ' saved as "'.$g."\".\n";
    }
}
else {
    $p=writeMacroHeader($extension,$procs);
    $h=writePrototypeHeader($extension,$procs,$tokens);
    $c=writeInitializationCode($extension);

    require_once 'index.php';
}

?>
