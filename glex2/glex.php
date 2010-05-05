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
             "       [api=<extension|version>] [debug=<verbosity>]\n\n".
             "Where:\n".
             "       <es>    is an enumerant value specification file like\n".
             "               http://www.opengl.org/registry/api/enumext.spec\n\n".
             "       <fs>    is a function specification file like\n".
             "               http://www.opengl.org/registry/api/gl.spec\n\n".
             "       <tm>    is a type map file like\n".
             "               http://www.opengl.org/registry/api/gl.tm\n\n".
             "       [api]   is a comma separated list of extension names like\n".
             "               \"ARB_transpose_matrix\" or version strings like \"VERSION_2_0\"\n".
             "               (optional)\n\n".
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
    $a=strtok($api,',');
    while ($a) {
        // The function header file needs to be written first because it is checked for later.
        $f=writeFunctionHeaderFile($fs_table,$a);

        if (!empty($f)) {
            writeEnumHeaderFile($es_table,$a,true);
            writeInitializationCodeFile($a);
        }
        else {
            writeEnumHeaderFile($es_table,$a,false);
        }

        $a=strtok(',');
    }

    $g='GLEX_globals.h';
    if (file_exists($g)==FALSE) {
        echo 'Copying globals header file ...';
        copy(dirname($argv[0]).'/'.$g,$g);
        echo " saved as \"$g\".\n";
    }
}

?>
