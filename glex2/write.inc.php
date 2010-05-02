<?php

require_once 'constants.inc.php';

function writeGlobalHeaderFile($table,$api) {
    if (empty($api)) {
        $file='glex.h';
    }
    else {
        $file='GLEX_'.$api.'.h';
    }

    // Write the inclusion guard header.
    $guard=strtoupper(strtr(basename($file),'.','_'));

    $contents.="#ifndef $guard\n";
    $contents.="#define $guard\n\n";

    function writeDefinesForAPI($defines,$api,&$contents) {
        if (empty($defines)) {
            return;
        }

        $contents.="// $api\n";

        // Find the longest define name.
        $max_length=0;
        foreach ($defines as $name => $value) {
            $length=strlen($name);
            if ($length>$max_length) {
                $max_length=$length;
            }
        }

        foreach ($defines as $name => $value) {
            $contents.='#define '.str_pad($name,$max_length).' '.$value."\n";
        }

        $contents.="\n";
    }

    if (empty($api)) {
        foreach ($table as $api => $defines) {
            writeDefinesForAPI($defines,$api,$contents);
        }
    }
    else {
        writeDefinesForAPI($table[$api],$api,$contents);
    }

    // Write the inclusion guard footer.
    $contents.="#endif // $guard\n";

    file_put_contents($file,$contents);
}

function writeInitializationCodeFile($api) {
    global $cmdline;

    $prefix=strtoupper(APP_NAME).'_';

    if (empty($api)) {
        $name=strtolower(APP_NAME);
    }
    else {
        $name=$prefix.$api;
    }

    $file=$name;
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }

    // There is nothing to do if we have no functions to initialize.
    if (!file_exists($file.'_funcs.h')) {
        return $contents;
    }

    $contents.="#include \"$name.h\"\n\n";

    $ignore=strtoupper($name).'_IGNORE';
    $contents.="#ifndef $ignore\n\n";

    // Write the code that generates the function pointer variables.
    $contents.="// Initialize all function pointers to 0.\n";
    $contents.='#define '.$prefix."FUNC(t,n,a) t (APIENTRY *$prefix##n) a=0\n";
    $contents.='    #include "'.$name.'_funcs.h"'."\n";
    $contents.='#undef '.$prefix."FUNC\n\n";

    $contents.="GLboolean $name=GL_FALSE;\n\n";

    // Write the code that initializes the function pointer variables.
    $contents.="// Get the addresses for all functions of this API.\n";
    $contents.='GLboolean '.$name."_init(void)\n{\n";
    $contents.="    $name=GL_TRUE;\n\n";

    $contents.='#define '.$prefix."FUNC(t,n,a) $name&=((*((FUNC*)&$prefix##n)=wglGetProcAddress(#n))!=0)\n";
    $contents.='    #include "'.$name.'_funcs.h"'."\n";
    $contents.='#undef '.$prefix."FUNC\n\n";

    $contents.="    return $name;\n";
    $contents.="}\n\n";

    $contents.="#endif // $ignore\n";

    if (!empty($contents)) {
        file_put_contents($file.'.c',$contents);
    }
}

?>
