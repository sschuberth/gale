<?php

require_once 'parse.inc.php';
require_once 'write.inc.php';

// To be on the safe side, assume "register_globals" is set to "off".
$argc=$_SERVER['argc'];
$argv=$_SERVER['argv'];

$cmdline=($argv[0]==$_SERVER['PHP_SELF'] || empty($_SERVER['PHP_SELF']));

if ($cmdline) {
    for ($i=1;$i<$argc;++$i) {
        // Make URL-encoded variables available in the current scope.
        parse_str($argv[$i]);
    }
}
else {
    // Use @ to suppress any errors in case optional arguments are missing.
    @$enumspec=$_REQUEST['es'];
    @$funcspec=$_REQUEST['fs'];
    @$tm=$_REQUEST['tm'];
    @$api=$_REQUEST['api'];
    @$debug=$_REQUEST['debug'];
}

if (empty($es) || empty($fs) || empty($tm)) {
    if ($cmdline) {
        // When run from the command line, show the usage information.
        exit("Usage:\n".
             "       ".basename($argv[0])." es=<enumspec> fs=<funcspec> tm=<typemap>\n".
             "       [api=(<name>[,name]|<@file>)] [debug=<verbosity>]\n\n".
             "Where:\n".
             "       <es>    is an enumerant value specification file like\n".
             "               http://www.opengl.org/registry/api/enumext.spec\n\n".
             "       <fs>    is a function specification file like\n".
             "               http://www.opengl.org/registry/api/gl.spec\n\n".
             "       <tm>    is a type map file like\n".
             "               http://www.opengl.org/registry/api/gl.tm\n\n".
             "       [api]   is either a comma separated list of extension names like\n".
             "               \"ARB_transpose_matrix\" and version strings like \"VERSION_2_0\"\n".
             "               or a file name prefixed by \"@\" with one name per line (optional)\n\n".
             "       [debug] is a debug verbosity level starting at 1, higher numbers\n".
             "               are more verbose (optional)\n\n"
        );
    }
    else {
        // If the script is run on a web server, show a form.
        header('Location: index.php');
        return;
    }
}

parseEnumSpec($es,$es_table);

// $tm_table must be globally available for the function specification parser callback function.
parseTypeMap($tm,$tm_table);
parseFuncSpec($fs,$fs_table);

if ($cmdline) {
    if ($api[0]=='@') {
        $file=ltrim($api,'@');
        if ($debug>=1) {
            echo "*** DEBUG *** Reading APIs from \"$file\".\n";
        }
        $contents=file_get_contents($file);
        $api=str_replace(array("\r","\n"),array('',','),$contents);
    }
}

$a=strtok($api,',');
while ($a) {
    if ($debug>=1) {
        echo "*** DEBUG *** Parsing API \"$a\".\n";
    }

    // The function header file needs to be written first because it is checked for later.
    $f=writeFunctionHeaderFile($fs_table,$a);

    if (!empty($f)) {
        $e=writeEnumHeaderFile($es_table,$a,true);
        $c=writeInitializationCodeFile($a);
    }
    else {
        $e=writeEnumHeaderFile($es_table,$a,false);
    }

    $a=strtok(',');
}

if ($cmdline) {
    $g='GLEX_globals.h';
    if (file_exists($g)==FALSE) {
        if ($debug>=1) {
            echo "*** DEBUG *** Copying globals header file to \"$g\".\n";
        }
        copy(dirname($argv[0]).'/'.$g,$g);
    }
}
else {
    require_once 'index.php';
}

?>
