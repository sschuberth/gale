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
             "       [api]   is either an extension name like \"ARB_transpose_matrix\" or\n".
             "               and an API version string like \"VERSION_2_0\" (optional)\n\n".
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

function parseFile($parser,$file,&$table) {
    // Parse the file into an associative array.
    $handle=@fopen($file,'r') or exit('Error: Unable to open the file "'.$file.'" for reading.');

    while (!feof($handle)) {
        $line=fgets($handle);

        // Strip comments.
        $pos=strpos($line,'#');
        if ($pos!==FALSE) {
            $line=substr($line,0,$pos);
        }

        $line=preg_replace('/\/\*.*\*\//','',$line);

        // Remove leading and trailing whitespaces.
        $line=trim($line);

        // If the line is empty now, there is nothing to do.
        if (empty($line)) {
            continue;
        }

        call_user_func_array($parser,array($line,&$table));
    }

    fclose($handle);
}

function callbackEnumSpec($line,&$table) {
    // Example:
    // passthru: /* AttribMask */
    if (strpos($line,'passthru:')===0) {
        return;
    }

    // Example:
    // VERSION_1_1 enum:
    if (preg_match('/(.+) enum:$/',$line,$matches)) {
        $table[$matches[1]]=array();
        end($table);
    }
    // Example:
    // 	DEPTH_BUFFER_BIT				= 0x00000100	# AttribMask
    else if (preg_match('/(\w+)\s*=\s*(\w+)$/',$line,$matches)) {
        $table[key($table)]['GL_'.$matches[1]]=$matches[2];
    }
    // Example:
    // 	use ARB_depth_buffer_float	    DEPTH_COMPONENT32F
    else if (preg_match('/use\s+(\w+)\s+(\w+)$/',$line,$matches)) {
        $table[key($table)]['GL_'.$matches[2]]=$matches[1];
    }
}

function callbackTypeMap($line,&$table) {
    // Example:
    // AccumOp,*,*,			    GLenum,*,*
    $line=str_replace(array(" ","\t"),array("",""),$line);
    $entry=explode(',',$line);
    $table[$entry[0]]=$entry[3];
}

function parseEnumSpec($file,&$table) {
    global $debug;

    if ($debug>=1) {
        echo "*** DEBUG parseEnumSpec() *** Opening \"$file\" for reading.\n";
    }

    parseFile('callbackEnumSpec',$file,$table);
    reset($table);

    // Resolve "use" directives.
    function resolveUseDirectives(&$value,$key,$table) {
        global $debug;

        if (is_array($value)) {
            return;
        }

        // If $value is an extension name, we get its defines, else return.
        $defines=$table[$value];
        if (empty($defines)) {
            return;
        }

        // We are done if we find a value.
        $value=$defines[$key];
        if (!empty($value)) {
            return;
        }

        if ($debug>=1) {
            echo "*** DEBUG resolveUseDirectives() *** \"$key\" not found.\n";
        }

        // Try some heuristics for $key.
        $vendors=array(
            '3DFX','3DL'
        ,   'AMD','APPLE','ARB','ATI'
        ,   'EXT'
        ,   'GREMEDY'
        ,   'HP'
        ,   'I3D','IBM','INGR','INTEL'
        ,   'MESA','MESAX'
        ,   'NV'
        ,   'OES','OML'
        ,   'PGI'
        ,   'REND'
        ,   'S3','SGI','SGIS','SGIX','SUN','SUNX'
        ,   'WIN'
        );

        $pos=strrpos($key,'_');
        $suffix=substr($key,$pos+1);
        if (in_array($suffix,$vendors)) {
            $key2=substr($key,0,$pos);
        }

        $value=$defines[$key2];
    }

    array_walk_recursive($table,'resolveUseDirectives',$table);

    if ($debug>=2) {
        echo "*** DEBUG parseEnumSpec() *** BEGIN\n";
        var_dump($table);
        echo "*** DEBUG parseEnumSpec() *** END\n";
    }
}

function parseTypeMap($file,&$table) {
    global $debug;

    if ($debug>=1) {
        echo "*** DEBUG parseTypeMap() *** Opening \"$file\" for reading.\n";
    }

    parseFile('callbackTypeMap',$file,$table);

    if ($debug>=2) {
        echo "*** DEBUG parseTypeMap() *** BEGIN\n";
        var_dump($table);
        echo "*** DEBUG parseTypeMap() *** END\n";
    }
}

parseEnumSpec($es,$es_table);

?>
