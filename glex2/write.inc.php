<?php

require_once 'constants.inc.php';

function writeFunctionHeaderFile($table,$api) {
    global $cmdline;

    $prefix=strtoupper(APP_NAME).'_';

    if (empty($api)) {
        $namespace=strtolower(APP_NAME);
    }
    else {
        $namespace=$prefix.$api;
    }

    $file=$namespace.'_funcs.inl';
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }

    function writeFuncsForAPI($prefix,$funcs,$api,&$contents) {
        if (empty($funcs)) {
            return;
        }

        if (!empty($contents)) {
            $contents.="\n";
        }

        $contents.="// $api\n";

        foreach ($funcs as $f) {
            $contents.="${prefix}FUNC( $f[0], $f[1], ($f[2]) );\n";
            $contents.="#ifndef $f[1]\n";
            $contents.="    #define $f[1] $prefix$f[1]\n";
            $contents.="#endif\n";
        }
    }

    if (empty($api)) {
        foreach ($table as $api => $funcs) {
            writeFuncsForAPI($prefix,$funcs,$api,$contents);
        }
    }
    else {
        writeFuncsForAPI($prefix,$table[$api],$api,$contents);
    }

    if (!empty($contents)) {
        file_put_contents($file,$contents);
    }
}

function writeEnumHeaderFile($table,$api) {
    global $cmdline;

    $prefix=strtoupper(APP_NAME).'_';

    if (empty($api)) {
        $namespace=strtolower(APP_NAME);
    }
    else {
        $namespace=$prefix.$api;
    }

    $file=$namespace.'.h';
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }

    // Write the inclusion guard header.
    $guard=strtoupper(strtr(basename($file),'.','_'));

    $contents.="#ifndef $guard\n";
    $contents.="#define $guard\n\n";

    $ignore=strtoupper($namespace).'_IGNORE';

    $contents.="#ifdef __linux\n";
    $contents.="    // TODO: Linux support.\n";
    if (strpos($guard,'_WGL_')!==FALSE) {
        $contents.="\n    // Ignore WGL extensions under Linux.\n";
        $contents.="    #define $ignore\n";
    }
    $contents.="#endif\n\n";

    $contents.="#ifdef _WIN32\n";
    $contents.="    #ifndef WIN32_LEAN_AND_MEAN\n";
    $contents.="        #define WIN32_LEAN_AND_MEAN 1\n";
    $contents.="    #endif\n";
    $contents.="    #include <windows.h>\n";
    if (strpos($guard,'_GLX_')!==FALSE) {
        $contents.="\n    // Ignore GLX extensions under Windows.\n";
        $contents.="    #define $ignore\n";
    }
    $contents.="#endif\n\n";

    $contents.="#ifndef $ignore\n\n";

    $contents.="#include <GL/gl.h>\n";
    $contents.="#include \"${prefix}globals.h\"\n\n";

    function writeEnumsForAPI($defines,$api,&$contents) {
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
            writeEnumsForAPI($defines,$api,$contents);
        }
    }
    else {
        writeEnumsForAPI($table[$api],$api,$contents);
    }

    if (file_exists(substr($file,0,strrpos($file,'.')).'_funcs.inl')) {
        $contents.="#ifdef __cplusplus\n";
        $contents.="extern \"C\"\n{\n";
        $contents.="#endif\n\n";

        $contents.="extern GLboolean ${namespace}_init(void);\n";
        $contents.="extern GLboolean $namespace;\n\n";

        // Write the code that generates the prototype definitions.
        $contents.="// List the pointer prototypes for all functions of this API.\n";
        $contents.="#define ${prefix}FUNC(t,n,a) extern t (APIENTRY *${prefix}##n) a\n";
        $contents.="    #include \"${namespace}_funcs.inl\"\n";
        $contents.="#undef ${prefix}FUNC\n\n";

        $contents.="#ifdef __cplusplus\n";
        $contents.="} // extern \"C\"\n";
        $contents.="#endif\n\n";
    }

    $contents.="#endif // $ignore\n\n";

    // Write the inclusion guard footer.
    $contents.="#endif // $guard\n";

    file_put_contents($file,$contents);
}

function writeInitializationCodeFile($api) {
    global $cmdline;

    $prefix=strtoupper(APP_NAME).'_';

    if (empty($api)) {
        $namespace=strtolower(APP_NAME);
    }
    else {
        $namespace=$prefix.$api;
    }

    $file=$namespace;
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }

    // There is nothing to do if we have no functions to initialize.
    if (!file_exists($file.'_funcs.inl')) {
        return;
    }

    $contents.="#include \"$namespace.h\"\n\n";

    $ignore=strtoupper($namespace).'_IGNORE';
    $contents.="#ifndef $ignore\n\n";

    // Write the code that generates the function pointer variables.
    $contents.="// Initialize all function pointers to 0.\n";
    $contents.="#define ${prefix}FUNC(t,n,a) t (APIENTRY *$prefix##n) a=0\n";
    $contents.="    #include \"${namespace}_funcs.inl\"\n";
    $contents.="#undef {$prefix}FUNC\n\n";

    $contents.="GLboolean $namespace=GL_FALSE;\n\n";

    // Write the code that initializes the function pointer variables.
    $contents.="// Get the addresses for all functions of this API.\n";
    $contents.="GLboolean ${namespace}_init(void)\n{\n";
    $contents.="    $namespace=GL_TRUE;\n\n";

    $contents.="#define ${prefix}FUNC(t,n,a) $namespace&=((*((FUNC*)&$prefix##n)=wglGetProcAddress(#n))!=0)\n";
    $contents.="    #include \"${namespace}_funcs.inl\"\n";
    $contents.="#undef ${prefix}FUNC\n\n";

    $contents.="    return $namespace;\n";
    $contents.="}\n\n";

    $contents.="#endif // $ignore\n";

    file_put_contents($file.'.c',$contents);
}

?>
