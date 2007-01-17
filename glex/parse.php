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
} else {
    // Suppress any errors in case these variables are not set.
    @$spec=$_REQUEST['spec'];
    @$debug=$_REQUEST['debug'];
}

if (empty($spec)) {
    if ($cmdline) {
        // When run from the command line, the spec has to be passed as an argument.
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file> [debug=<verbosity level 1 or 2>]');
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
    echo "*** debug *** Dumping section headers\n";
    foreach (array_keys($struct) as $section) {
        // Dump only the section header.
        echo $section."\n";
    }
} else if ($debug==2) {
    // Dump the array containing the file structure if debugging is enabled.
    echo "*** debug *** Dumping file structure array\n";
    print_r($struct);
}

// Search for the section where new procedures and functions are defined.
$remaining=2;
foreach (array_keys($struct) as $section) {
    // We need to find strings like these (and variations):
    // "New Procedures and Functions"
    // "New Procedures, Functions and Structures:"
    if (preg_match('/New\W+Procedures\W+\w*\W*Functions/',$section)>0) {
        $procs=$struct[$section];
        if ($debug==1) {
            echo "*** debug *** Dumping procedures and functions section content\n";
            echo $procs."\n";
        }
        --$remaining;
    }

    if (preg_match('/New\W+Tokens/',$section)>0) {
        $tokens=$struct[$section];
        if ($debug==1) {
            echo "*** debug *** Dumping tokens section content\n";
            echo $tokens."\n";
        }
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

if ($cmdline) {
    echo 'Writing macro header ...';
    $p=writeMacroHeader($extension,$procs);
    echo ' saved as "'.$p."\".\n";

    echo 'Writing prototype header ...';
    $h=writePrototypeHeader($extension,$procs,$tokens);
    echo ' saved as "'.$h."\".\n";

    echo 'Writing initialization code ...';
    $c=writeInitializationCode($extension);
    echo ' saved as "'.$c."\".\n";

    $g='GLEX_globals.h';
    if (file_exists($g)===FALSE) {
        echo 'Copying globals header ...';
        copy(dirname($argv[0]).'/'.$g,$g);
        echo ' saved as "'.$g."\".\n";
    }
} else {
    $p=writeMacroHeader($extension,$procs);
    $h=writePrototypeHeader($extension,$procs);
    $c=writeInitializationCode($extension);

    require_once 'index.php';
}

?>
