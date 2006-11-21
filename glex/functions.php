<?php

require_once 'constants.php';

function parseSpecIntoArray($spec,&$struct) {
    // Parse the sections and contents of the file into an associative array
    // that represents the file structure.
    $handle=@fopen($spec,'r') or
        exit("Error: Unable to open the file \"$spec\" for reading.");

    $section=NULL;
    $content=NULL;

    $i=0;
    while (!feof($handle)) {
        $line=rtrim(fgets($handle));
        ++$i;

        if (strlen($line)>OPENGL_SPEC_MAX_CHARS_PER_LINE)
            echo "Warning: Line $i exceeds the maximum of ".OPENGL_SPEC_MAX_CHARS_PER_LINE." characters.\n";

        // We can stop parsing when we have reached e.g. the revision history.
        if (is_int(strpos($line,OPENGL_SPEC_BREAK_STRING)))
            break;

        // Skip empty lines right from the start.
        $ltline=ltrim($line);
        if (empty($ltline))
            continue;

        if ($line!=$ltline) {
            // Switch to content parsing mode until a new section header is found.
            if (!empty($content))
                $content.=' ';
            $content.=$ltline;
        } else {
            // Skip comments as shown in the OpenGL extension document template.
            if ($line[0]=='*')
                continue;

            // Add any new section and (possibly empty) content to the structure array.
            if (!is_null($section) && !is_null($content)) {
                $struct[$section]=$content;
                $section=NULL;
                $content=NULL;
            }

            // Switch to section parsing mode until indented text in found.
            if (!empty($section))
                $section.=' ';
            $section.=$line;
        }
    }
    $struct[$section]=$content;

    fclose($handle);
}

function writeMacroHeader($extension,$content) {
    global $local;

    $file=$extension.'_procs.h';
    if (!$local)
        $file=SERVER_TMP_DIRECTORY.$file;
    $handle=fopen($file,'w');

    // TODO: Verify / simplify these regular expressions.
    $type="\w+\s*\*?\w+\s*\*?";
    $name="\w+";
    $arguments="($type\s*,?\s*)*";
    preg_match_all("/($type)\s+($name)\s*\(($arguments)\)\s*;/",$content,$matches,PREG_SET_ORDER);

    $type_length_max=0;
    $name_length_max=0;
    $arguments_length_max=0;

    foreach ($matches as $procedure) {
        list($all,$type,$name,$arguments,$argument)=$procedure;

        // Get the maximum string lengths for alignment.
        $length=strlen($type);
        if ($type_length_max<$length)
            $type_length_max=$length;

        $length=strlen($name);
        if ($name_length_max<$length)
            $name_length_max=$length;

        $length=strlen($arguments);
        if ($arguments_length_max<$length)
            $arguments_length_max=$length;
    }

    foreach ($matches as $procedure) {
        list($all,$type,$name,$arguments,$argument)=$procedure;

        // Write the padded function prototypes to a header file.
        $type=str_pad($type,$type_length_max+1);
        $name=str_pad($name,$name_length_max+1);
        $arguments=str_pad('('.$arguments.')',$arguments_length_max+2);
        fwrite($handle,"GLEX_PROC( $type, $name, $arguments );\n");
    }

    fclose($handle);

    return $file;
}

function extractTypesToString($content,&$types) {
    preg_match_all("/DECLARE_HANDLE.*(.*).*;/U",$content,$matches,PREG_SET_ORDER);
    foreach ($matches as $type) {
        list($all)=$type;
        $types.="$all\n\n";
    }

    preg_match_all("/typedef(\s+\w+)+\s*;/U",$content,$matches,PREG_SET_ORDER);
    foreach ($matches as $type) {
        list($all)=$type;
        $types.="$all\n\n";
    }

    preg_match_all("/typedef.*{.*}.*;/U",$content,$matches,PREG_SET_ORDER);
    foreach ($matches as $type) {
        list($all)=$type;
        $types.="$all\n\n";
    }
}

function writePrototypeHeader($extension,$content) {
    global $local;

    $file=$extension.'.h';
    if (!$local)
        $file=SERVER_TMP_DIRECTORY.$file;
    $handle=fopen($file,'w');
    $guard=strtoupper(strtr(basename($file),'.','_'));

    // Write the inclusion guard header.
    fwrite($handle,"#ifndef $guard\n");
    fwrite($handle,"#define $guard\n\n");

    fwrite($handle,"#define WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"    #include <windows.h>\n");
    fwrite($handle,"#undef WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"#include <GL/gl.h>\n\n");

    fwrite($handle,"#ifdef __cplusplus\n");
    fwrite($handle,"extern \"C\" {\n");
    fwrite($handle,"#endif\n\n");

    fwrite($handle,'extern GLboolean '.$extension."_init(void);\n\n");

    extractTypesToString($content,$types);
    if (!empty($types)) {
        fwrite($handle,"// List the data types introduced by this extension.\n");
        fwrite($handle,$types);
    }

    // Write the code that generates the prototype definitions.
    fwrite($handle,"// List the pointer prototypes for all functions of this extension.\n");
    fwrite($handle,"#define GLEX_PROC(t,n,a) extern t (APIENTRY *n) a\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,"#undef GLEX_PROC\n\n");

    fwrite($handle,"#ifdef __cplusplus\n");
    fwrite($handle,"} // extern \"C\"\n");
    fwrite($handle,"#endif\n\n");

    // Write the inclusion guard footer.
    fwrite($handle,"#endif // $guard\n");

    fclose($handle);

    return $file;
}

function writeInitializationCode($extension) {
    global $local;

    $file=$extension.'.c';
    if (!$local)
        $file=SERVER_TMP_DIRECTORY.$file;
    $handle=fopen($file,'w');

    fwrite($handle,'#include "'.$extension.'.h"'."\n\n");

    // Write the code that generates the function pointer variables.
    fwrite($handle,"// Initialize all function pointers to 0.\n");
    fwrite($handle,"#define GLEX_PROC(t,n,a) t (APIENTRY *n) a=0\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,"#undef GLEX_PROC\n\n");

    // Write the code that initializes the function pointer variables.
    fwrite($handle,"// Get the addresses for all functions of this extension.\n");
    fwrite($handle,'GLboolean '.$extension."_init(void) {\n");
    fwrite($handle,"    GLboolean r=GL_TRUE;\n\n");

    fwrite($handle,"#define GLEX_PROC(t,n,a) r=r&&((n=(void*)wglGetProcAddress(#n))!=0)\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,"#undef GLEX_PROC\n\n");

    fwrite($handle,"    return r;\n");
    fwrite($handle,"}\n");

    fclose($handle);

    return $file;
}

?>
