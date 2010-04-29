<?php

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
    @$enumspec=$_REQUEST['enumspec'];
    @$funcspec=$_REQUEST['funcspec'];
    @$tm=$_REQUEST['tm'];
    @$api=$_REQUEST['api'];
    @$debug=$_REQUEST['debug'];
}

if (empty($enumspec) || empty($funcspec) || empty($tm)) {
    if ($cmdline) {
        // When run from the command line, show the usage information.
        exit("Usage:\n".
             "       ".basename($argv[0])." enumspec=<enumspec> funcspec=<funcspec> tm=<typemap>\n".
             "       [api=<extension|version>] [debug=<verbosity>]\n\n".
             "Where:\n".
             "       <enumspec> is an enumerant value specification file like\n".
             "                   http://www.opengl.org/registry/api/enumext.spec\n\n".
             "       <funcspec> is a function specification file like\n".
             "                  http://www.opengl.org/registry/api/gl.spec\n\n".
             "       <tm>       is a type map file like\n".
             "                  http://www.opengl.org/registry/api/gl.tm\n\n".
             "       [api]      is either an extension name like \"ARB_transpose_matrix\" or\n".
             "                  and an API version string like \"GL_VERSION_2_0\" (optional)\n\n".
             "       [debug]    is a debug verbosity level starting at 1, higher numbers\n".
             "                  are more verbose (optional)\n\n"
        );
    }
    else {
        // If the script is run on a web server, show a form.
        header('Location: index.php');
        return;
    }
}

function parseEnumSpec($file,&$table) {
    global $debug;
}

function parseTypeMap($file,&$table) {
    global $debug;

    if ($debug>=1) {
        echo "*** DEBUG parseTypeMap() *** Opening $file for reading.\n";
    }

    // Parse http://www.opengl.org/registry/api/gl.tm into an associative array.
    $handle=@fopen($file,'r') or exit('Error: Unable to open the file "'.$file.'" for reading.');

    while (!feof($handle)) {
        // Trim any, also inner, whitespaces.
        $line=str_replace(array(" ","\t"),array("",""),fgets($handle));

        // Skip comments and empty lines right from the start.
        if (empty($line) || $line[0]=='#') {
            continue;
        }

        // A line looks like this:
        // AccumOp,*,*,			    GLenum,*,*
        $entry=explode(',',$line);
        $table[$entry[0]]=$entry[3];
    }

    fclose($handle);

    if ($debug>=2) {
        echo "*** DEBUG parseTypeMap() *** BEGIN\n";
        foreach ($table as $key => $value) {
            echo "Map $key to $value.\n";
        }
        echo "*** DEBUG parseTypeMap() *** END\n";
    }
}

parseTypeMap($tm,$typemap);

?>
