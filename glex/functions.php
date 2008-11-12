<?php

require_once 'constants.php';

function getFunctionsRevision() {
    return '$Revision$';
}

function getFunctionsDate() {
    return '$Date$';
}

/*
 * Parsing Oddities
 * ----------------
 *
 * Non-standard section caption for new procedures and functions, incl. a
 * trailing colon:
 * http://developer.download.nvidia.com/opengl/specs/WGL_nv_gpu_affinity.txt
 *
 * No trailing semicolons and multi-line procedure declarations:
 * http://oss.sgi.com/projects/ogl-sample/registry/ARB/shader_objects.txt
 *
 * New GLX token specified with 8 hex-digits:
 * http://www.opengl.org/registry/specs/ARB/color_buffer_float.txt
 *
 */

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

        // Skip comments and empty lines right from the start.
        $ltline=ltrim($line);
        if ($line[0]=='*' || empty($ltline)) {
            continue;
        }

        // Be sure to make a string comparison even if the strings only consist
        // of numbers.
        if ($line!==$ltline) {
            // Switch to content parsing mode until a new section header is found.
            if (!empty($content)) {
                $content.=' ';
            }
            $content.=$ltline;
        }
        else {
            // We can stop parsing when we have reached e.g. the revision history.
            if (is_int(strpos($line,OPENGL_SPEC_BREAK_SECTION))) {
                break;
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

function writeMacroHeader($extension,$procs) {
    function addDataTypePrefix(&$arguments) {
        $patterns=array(
            "/(^|\W)(boolean)/",
            "/(^|\W)(byte)/",
            "/(^|\W)(ubyte)/",
            "/(^|\W)(char)/",
            "/(^|\W)(short)/",
            "/(^|\W)(ushort)/",
            "/(^|\W)(int)/",
            "/(^|\W)(uint)/",
            "/(^|\W)(sizei)/",
            "/(^|\W)(enum)/",
            "/(^|\W)(bitfield)/",
            "/(^|\W)(half)/",
            "/(^|\W)(float)/",
            "/(^|\W)(clampf)/",
            "/(^|\W)(double)/",
            "/(^|\W)(clampd)/",
            "/(^|\W)(time)/",
            "/(^|\W)(void)/",
            "/(^|\W)(\w+ARB)/"
        );
        $result=preg_replace($patterns,"\\1GL\\2",$arguments);
        if ($result===NULL || $result===$arguments) {
            return FALSE;
        }
        $arguments=$result;
        return TRUE;
    }

    global $cmdline;

    // Real-world argument examples:
    // enum texunit
    // const float *m
    // void* *params
    $type="\w+\s*\**\s*\**\w+\s*\**\s*\**";
    $name="\w+";
    $arguments="($type\s*,?\s*)*";

    preg_match_all("/($type)\s+($name)\s*\(($arguments)\)\s*;?/",$procs,$matches,PREG_SET_ORDER);

    $count=count($matches);
    for ($i=0;$i<$count;++$i) {
        // For custom data types, prepend "GL".
        $replaced1=addDataTypePrefix($matches[$i][1]);

        // If there is no lower case prefix, prepend "gl" to the name.
        $match=&$matches[$i][2];
        if ($match[0]>='A' && $match[0]<='Z') {
            $match='gl'.$match;
        }

        // For custom data types, prepend "GL".
        $replaced3=addDataTypePrefix($matches[$i][3]);

        // Check that we have at least once prefixed an OpenGL data type (e.g.
        // the return type), else remove the invalid match.
        if (!$replaced1 && !$replaced3) {
            // Note that $i is not an index but a key into the array. This means
            // deleting an entry will not make the following entries "move up".
            unset($matches[$i]);
        }
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

        //$arguments_pad=str_pad('('.$arguments.')',$arguments_length_max+2);
        //fwrite($handle,GLEX_PREFIX."PROC( $type_pad, $name_pad, $arguments_pad );\n");
        fwrite($handle,GLEX_PREFIX."PROC( $type_pad, $name_pad, ($arguments) );\n");

        fwrite($handle,"#ifndef $name\n");
        fwrite($handle,"    #define $name_pad".GLEX_PREFIX."$name\n");
        fwrite($handle,"#endif\n");
    }

    fclose($handle);

    return $file;
}

function writePrototypeHeader($extension,$procs,$tokens) {
    function extractTokensToString($tokens,&$defines) {
        $name_length_max=0;
        preg_match_all("/(\w+)\s+(0x[0-9a-fA-F]+)/",$tokens,$matches,PREG_SET_ORDER);

        for ($i=0;$i<count($matches);++$i) {
            $match=&$matches[$i][1];
            if (!preg_match("/^W?GLX?_/",$match)) {
                $match='GL_'.$match;
            }

            $name_length=strlen($match);
            if ($name_length>$name_length_max) {
                $name_length_max=$name_length;
            }
        }

        foreach ($matches as $define) {
            // Convert the array entries to strings.
            list(,$name,$number)=$define;
            $defines.='#define '.str_pad($name,$name_length_max+1).$number."\n";
        }

        if (!empty($defines)) {
            $defines.="\n";
        }
    }

    function extractTypesToString($procs,&$types) {
        preg_match_all("/DECLARE_HANDLE.*(.*).*;/U",$procs,$matches,PREG_SET_ORDER);
        foreach ($matches as $type) {
            // Convert the array to a string and append it.
            list($all)=$type;
            $types.="$all\n\n";
        }

        preg_match_all("/typedef(\s+\w+)+\s*;/U",$procs,$matches,PREG_SET_ORDER);
        foreach ($matches as $type) {
            // Convert the array to a string and append it.
            list($all)=$type;
            $types.="$all\n\n";
        }

        preg_match_all("/(typedef.*{)(.*)(}.*;)/U",$procs,$matches,PREG_SET_ORDER);
        foreach ($matches as $type) {
            // Indent the inner of a structure and append it.
            $all=$type[1]."\n";
            $tok=strtok($type[2],';');
            while ($tok!==FALSE) {
                $trimtok=trim($tok);
                if (!empty($trimtok)) {
                    $all.='    '.$trimtok.";\n";
                }
                $tok=strtok(';');
            }
            $all.=$type[3];
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

    $ignore=strtoupper($extension).'_IGNORE';

    fwrite($handle,"#ifdef _WIN32\n");
    fwrite($handle,"    #ifndef WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"        #define WIN32_LEAN_AND_MEAN\n");
    fwrite($handle,"        #include <windows.h>\n");
    fwrite($handle,"        #undef WIN32_LEAN_AND_MEAN\n");
    if (strpos($guard,"_GLX_")!==FALSE) {
        fwrite($handle,"\n        // Ignore GLX extensions under Windows.\n");
        fwrite($handle,"        #define $ignore\n");
    }
    fwrite($handle,"    #else\n");
    fwrite($handle,"        #include <windows.h>\n");
    fwrite($handle,"    #endif\n");
    fwrite($handle,"#else\n");
    fwrite($handle,"    #ifdef __linux\n");
    fwrite($handle,"        // TODO: Linux support.\n");
    if (strpos($guard,"_WGL_")!==FALSE) {
        fwrite($handle,"\n        // Ignore WGL extensions under Linux.\n");
        fwrite($handle,"        #define $ignore\n");
    }
    fwrite($handle,"    #endif\n");
    fwrite($handle,"#endif\n\n");

    fwrite($handle,"#ifndef $ignore\n\n");

    fwrite($handle,"#include <GL/gl.h>\n");
    fwrite($handle,"#include \"GLEX_globals.h\"\n\n");

    extractTokensToString($tokens,$defines);
    if (!empty($defines)) {
        fwrite($handle,"// List the defined tokens introduced by this extension.\n");
        fwrite($handle,$defines);
    }

    extractTypesToString($procs,$types);
    if (!empty($types)) {
        fwrite($handle,"// List the data types introduced by this extension.\n");
        fwrite($handle,$types);
    }

    fwrite($handle,"#ifdef __cplusplus\n");
    fwrite($handle,"extern \"C\"\n{\n");
    fwrite($handle,"#endif\n\n");

    fwrite($handle,'extern GLboolean '.$extension."_init(void);\n");
    fwrite($handle,"extern GLboolean $extension;\n\n");

    // Write the code that generates the prototype definitions.
    fwrite($handle,"// List the pointer prototypes for all functions of this extension.\n");
    fwrite($handle,'#define '.GLEX_PREFIX.'PROC(t,n,a) extern t (APIENTRY *'.GLEX_PREFIX."##n) a\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,'#undef '.GLEX_PREFIX."PROC\n\n");

    fwrite($handle,"#ifdef __cplusplus\n");
    fwrite($handle,"} // extern \"C\"\n");
    fwrite($handle,"#endif\n\n");

    fwrite($handle,"#endif // $ignore\n\n");

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

    $ignore=strtoupper($extension).'_IGNORE';
    fwrite($handle,"#ifndef $ignore\n\n");

    // Write the code that generates the function pointer variables.
    fwrite($handle,"// Initialize all function pointers to 0.\n");
    fwrite($handle,'#define '.GLEX_PREFIX.'PROC(t,n,a) t (APIENTRY *'.GLEX_PREFIX."##n) a=0\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,'#undef '.GLEX_PREFIX."PROC\n\n");

    fwrite($handle,"GLboolean $extension=GL_FALSE;\n\n");

    // Write the code that initializes the function pointer variables.
    fwrite($handle,"// Get the addresses for all functions of this extension.\n");
    fwrite($handle,'GLboolean '.$extension."_init(void)\n{\n");
    fwrite($handle,"    $extension=GL_TRUE;\n\n");

    fwrite($handle,'#define '.GLEX_PREFIX."PROC(t,n,a) $extension&=((*((PROC*)&".GLEX_PREFIX."##n)=wglGetProcAddress(#n))!=0)\n");
    fwrite($handle,'    #include "'.$extension.'_procs.h"'."\n");
    fwrite($handle,'#undef '.GLEX_PREFIX."PROC\n\n");

    fwrite($handle,"    return $extension;\n");
    fwrite($handle,"}\n\n");

    fwrite($handle,"#endif // $ignore\n");

    fclose($handle);

    return $file;
}

function drawTableBorder($prefix,$color,$edge=FALSE,$colspan=1) {
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
        <td><a href="download.php?file='.$f.'">'.$f.'</a>&nbsp;(<a href="download.php?file='.$f.'&amp;unix2dos=1">DOS&nbsp;format</a>)</td>
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
