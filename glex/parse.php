<?php

require 'functions.php';

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

parseSpecIntoArray($spec,$struct);
//print_r($struct);

// Search for the section where new procedures and functions are defined.
foreach (array_keys($struct) as $section) {
    if (preg_match('/New\W+Procedures\W+\w*\W*Functions/',$section)>0) {
        $content=$struct[$section];
        break;
    }
}

if (!empty($content)) {
    $type="\w+\s*\*?\w+\s*\*?";
    $name="\w+";
    $arguments="($type\s*,?\s*)*";
    preg_match_all("/($type)\s+($name)\s*\(($arguments)\)\s*;/",$content,$matches,PREG_SET_ORDER);

    $handle=fopen(basename($spec,'.txt').'_procs.h','w');

    foreach ($matches as $procedure) {
        list($all,$type,$name,$arguments,$argument)=$procedure;
        fwrite($handle,"PROC($type,$name,($arguments));\n");
    }
    
    fclose($handle);
}

?>
