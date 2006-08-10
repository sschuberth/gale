<?php

require_once 'functions.php';

// Assume "register_globals" is set to "Off".
$argc=$_SERVER['argc'];
$argv=$_SERVER['argv'];

@$spec=$_REQUEST['spec'];

if (empty($spec)) {
    if (empty($argv[0])) {
        // If the script is run on a web server, prompt for the spec.
        require_once 'form.php';
        return;
    } else {
        // When run from the command line, the spec has to be passed as an argument.
        exit('Usage: '.basename($argv[0]).' spec=<URI or URL to OpenGL extension specification text file>');
    }
}

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
