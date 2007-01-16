<?php

require_once 'constants.php';

function parseSpecIntoArray($spec,&$struct) {
    // Parse the sections and contents of the file into an associative array
    // that represents the file structure.
    $handle=@fopen($spec,'r') or
        exit('Error: Unable to open the file "'.$spec.'" for reading.');

    $section=NULL;
    $content=NULL;

    $i=0;
    while (!feof($handle)) {
        $line=rtrim(fgets($handle));
        ++$i;

        if (strlen($line)>OPENGL_SPEC_MAX_CHARS_PER_LINE) {
            echo "Warning: Line $i exceeds the maximum of ".OPENGL_SPEC_MAX_CHARS_PER_LINE." characters.\n";
        }

        // We can stop parsing when we have reached e.g. the revision history.
        if (is_int(strpos($line,OPENGL_SPEC_BREAK_STRING))) {
            break;
        }

        // Skip empty lines right from the start.
        $ltline=ltrim($line);
        if (empty($ltline)) {
            continue;
        }

        if ($line!=$ltline) {
            // Switch to content parsing mode until a new section header is found.
            if (!empty($content)) {
                $content.=' ';
            }
            $content.=$ltline;
        } else {
            // Skip comments as shown in the OpenGL extension document template.
            if ($line[0]=='*') {
                continue;
            }

            // Add any new section and (possibly empty) content to the structure array.
            if (!is_null($section) && !is_null($content)) {
                $struct[$section]=$content;
                $section=NULL;
                $content=NULL;
            }

            // Switch to section parsing mode until indented text in found.
            if (!empty($section)) {
                $section.=' ';
            }
            $section.=$line;
        }
    }
    $struct[$section]=$content;

    fclose($handle);
}

function writeMacroHeader($extension,$content) {
    function addDataTypePrefix(&$arguments) {
        $patterns=array(
            "/(^|\W)(\w+ARB)/",
            "/(^|\W)(boolean)/",
            "/(^|\W)(double)/",
            "/(^|\W)(enum)/",
            "/(^|\W)(float)/",
            "/(^|\W)(uint)/",
            "/(^|\W)(int)/",
            "/(^|\W)(ushort)/",
            "/(^|\W)(short)/",
            "/(^|\W)(sizei)/"
        );
        $arguments=preg_replace($patterns,"\\1GL\\2",$arguments);
    }

    global $cmdline;

    /*
     * Oddities
     * --------
     *
     * Non-standard section caption for new procedures and functions, incl. a
     * trailing colon:
     * http://developer.download.nvidia.com/opengl/specs/WGL_nv_gpu_affinity.txt
     *
     * No trailing semicolons and multi-line procedure declarations:
     * http://oss.sgi.com/projects/ogl-sample/registry/ARB/shader_objects.txt
     *
     */

    // TODO: Verify / simplify these regular expressions.
    $type="\w+\s*\**\w+\s*\**";
    $name="\w+";
    $arguments="($type\s*,?\s*)*";
    preg_match_all("/($type)\s+($name)\s*\(($arguments)\)\s*;?/",$content,$matches,PREG_SET_ORDER);

    for ($i=0;$i<count($matches);++$i) {
        // For custom data types, prepend "GL".
        addDataTypePrefix($matches[$i][1]);

        // If there is no lower case prefix, prepend "gl" to the name.
        $match=&$matches[$i][2];
        if ($match[0]>='A' && $match[0]<='Z') {
            $match='gl'.$match;
        }

        // For custom data types, prepend "GL".
        addDataTypePrefix($matches[$i][3]);
    }

    $type_length_max=0;
    $name_length_max=0;
    $arguments_length_max=0;

    // Get the maximum string lengths for alignment.
    foreach ($matches as $procedure) {
        list($all,$type,$name,$arguments,$argument)=$procedure;

        $length=strlen($type);
        if ($type_length_max<$length) {
            $type_length_max=$length;
        }

        $length=strlen($name);
        if ($name_length_max<$length) {
            $name_length_max=$length;
        }

        $length=strlen($arguments);
        if ($arguments_length_max<$length) {
            $arguments_length_max=$length;
        }
    }

    $file=$extension.'_procs.h';
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }
    $handle=fopen($file,'w');

    $i=0;
    foreach ($matches as $procedure) {
        if ($i++>0) {
            fwrite($handle,"\n");
        }

        list($all,$type,$name,$arguments,$argument)=$procedure;

        // Write the padded function prototypes to a header file.
        $type_pad=str_pad($type,$type_length_max+1);
        $name_pad=str_pad($name,$name_length_max+1);
        $arguments_pad=str_pad('('.$arguments.')',$arguments_length_max+2);
        fwrite($handle,GLEX_PREFIX."PROC( $type_pad, $name_pad, $arguments_pad );\n");
        fwrite($handle,"#ifndef $name\n");
        fwrite($handle,"    #define $name_pad".GLEX_PREFIX."$name\n");
        fwrite($handle,"#endif\n");
    }

    fclose($handle);

    return $file;
}

function writePrototypeHeader($extension,$content) {
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

    global $cmdline;

    $file=$extension.'.h';
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }
    $handle=fopen($file,'w');

    $guard=strtoupper(strtr(basename($file),'.','_'));

    // Write the inclusion guard header.
    fwrite($handle,"#ifndef $guard\n");
    fwrite($handle,"#define $guard\n\n");

    fwrite($handle,"#ifdef _WIN32\n");
    fwrite($handle,"    #ifndef WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"        #define WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"        #include <windows.h>\n");
    fwrite($handle,"        #undef WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"    #else\n");
    fwrite($handle,"        #include <windows.h>\n");
    fwrite($handle,"    #endif\n");
    fwrite($handle,"#endif\n\n");

    fwrite($handle,"#include <GL/gl.h>\n\n");

    fwrite($handle,"#include \"GLEX_globals.h\"\n\n");

    fwrite($handle,"#ifdef __cplusplus\n");
    fwrite($handle,"extern \"C\" {\n");
    fwrite($handle,"#endif\n\n");

    fwrite($handle,'extern GLboolean '.$extension."_init(void);\n");
    fwrite($handle,"extern GLboolean $extension;\n\n");

    extractTypesToString($content,$types);
    if (!empty($types)) {
        fwrite($handle,"// List the data types introduced by this extension.\n");
        fwrite($handle,$types);
    }

    // Write the code that generates the prototype definitions.
    fwrite($handle,"// List the pointer prototypes for all functions of this extension.\n");
    fwrite($handle,'#define '.GLEX_PREFIX.'PROC(t,n,a) extern t (APIENTRY *'.GLEX_PREFIX."##n) a\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,'#undef '.GLEX_PREFIX."PROC\n\n");

    fwrite($handle,"#ifdef __cplusplus\n");
    fwrite($handle,"} // extern \"C\"\n");
    fwrite($handle,"#endif\n\n");

    // Write the inclusion guard footer.
    fwrite($handle,"#endif // $guard\n");

    fclose($handle);

    return $file;
}

function writeInitializationCode($extension) {
    global $cmdline;

    $file=$extension.'.c';
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }
    $handle=fopen($file,'w');

    fwrite($handle,'#include "'.$extension.'.h"'."\n\n");

    // Write the code that generates the function pointer variables.
    fwrite($handle,"// Initialize all function pointers to 0.\n");
    fwrite($handle,'#define '.GLEX_PREFIX.'PROC(t,n,a) t (APIENTRY *'.GLEX_PREFIX."##n) a=0\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,'#undef '.GLEX_PREFIX."PROC\n\n");

    fwrite($handle,"GLboolean $extension=GL_FALSE;\n\n");

    // Write the code that initializes the function pointer variables.
    fwrite($handle,"// Get the addresses for all functions of this extension.\n");
    fwrite($handle,'GLboolean '.$extension."_init(void) {\n");
    fwrite($handle,"    $extension=GL_TRUE;\n\n");

    fwrite($handle,'#define '.GLEX_PREFIX."PROC(t,n,a) $extension&=((*((void**)&".GLEX_PREFIX."##n)=(void*)wglGetProcAddress(#n))!=0)\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,'#undef '.GLEX_PREFIX."PROC\n\n");

    fwrite($handle,"    return $extension;\n");
    fwrite($handle,"}\n");

    fclose($handle);

    return $file;
}

function drawTableBorder($prefix,$color,$edge=false,$colspan=1) {
    $color_url=urlencode($color);

    echo '<tr>';
    echo '<td class="corner"><img src="images/corner-'.$prefix.'l-'.$color_url.'.png" alt="" /></td>';

    echo '<td colspan="'.$colspan.'" style="width: 100%; background-color: '.$color;
    if ($edge) {
        echo '; background-image: url(images/edge-'.$prefix.'.png); background-repeat: repeat-x';
    }
    echo '"></td>';

    echo '<td class="corner"><img src="images/corner-'.$prefix.'r-'.$color_url.'.png" alt="" /></td>';
    echo '</tr>';
}

function showSourceCode($file) {
    global $geshi;

    $f=basename($file);
    echo '
    <tr>
        <td><!-- Corner spacer --></td>
        <td><a href="download.php?file='.$f.'">'.$f.'</a>&nbsp;(<a href="download.php?file='.$f.'&unix2dos=1">DOS&nbsp;format</a>)</td>
        <td style="width: 100%; padding-left: 10px"><hr /></td>
        <td><!-- Corner spacer --></td>
    </tr>
    ';

    echo '<tr>';
    echo '<td><!-- Corner spacer --></td>';

    $geshi->load_from_file($file);
    $geshi->set_language("cpp");
    echo '<td colspan="2">'.$geshi->parse_code().'</td>';

    echo '<td><!-- Corner spacer --></td>';
    echo '</tr>';
}

?>
