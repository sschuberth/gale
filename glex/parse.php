<?php

require_once 'functions.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];
$argv=$_SERVER['argv'];

// Suppress any errors in case these variables are not set.
@$spec=$_REQUEST['spec'];
@$debug=$_REQUEST['debug'];

$local=($argv[0]==$_SERVER['PHP_SELF']);

if (empty($spec)) {
    if ($local) {
        // When run from the command line, the spec has to be passed as an argument.
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file>');
    } else {
        // If the script is run on a web server, prompt for the spec.
        header('Location: index.html');
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

$p=writeMacroHeader($extension,$content);
$h=writePrototypeHeader($extension,$content);
$c=writeInitializationCode($extension);

//copy($p,"/home/users/eyebex/gale/glex/cache/a.txt");

/*
$zip=new ZipArchive();
if ($zip->open('/home/users/eyebex/gale/glex/cache/'.$extension.'.zip',ZIPARCHIVE::CREATE)!==TRUE)
   echo("Unable to create ZIP archive \"$filename\".");
$zip->addFile($p);
$zip->addFile($h);
$zip->addFile($c);
$zip->close();
*/

//header('Location: '.SERVER_TMP_DIRECTORY.$extension.'_procs.h');

$handle=fopen($p,"r");
echo fgets($handle);
fclose($handle);

?>
