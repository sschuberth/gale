<?php

require_once 'constants.php';

function getFunctionsRevision() {
    return '$Revision$';
}

function getFunctionsDate() {
    return '$Date$';
}

/*
 * Examples for parsing oddities:
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
    global $cmdline;

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

        if ($cmdline && strlen($line)>OPENGL_SPEC_MAX_CHARS_PER_LINE) {
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
            "/(^|\W)(handle)/",
            "/(^|\W)(sizei)/",
            "/(^|\W)(enum)/",
            "/(^|\W)(bitfield)/",
            "/(^|\W)(half)/",
            "/(^|\W)(float)/",
            "/(^|\W)(clampf)/",
            "/(^|\W)(double)/",
            "/(^|\W)(clampd)/",
            "/(^|\W)(time)/",
            "/(^|\W)(void)/"
        );
        $result=preg_replace($patterns,"\\1GL\\2",$arguments);
        if ($result===NULL || $result===$arguments) {
            return FALSE;
        }
        $arguments=$result;
        return TRUE;
    }

    global $cmdline;

    // First, roughly split into the parts outside and inside of the parentheses.
    preg_match_all("/\s*([^\(\)]+)\s*\(([^\(\)]*)\)\s*;?/",$procs,$matches_proc,PREG_SET_ORDER);

    for ($i=0;$i<count($matches_proc);++$i) {
        // Second, separate the return type and procedure name.
        preg_match("/^(.+)(\w+?)$/U",$matches_proc[$i][1],$matches_line);

        // Trim whitespaces from the return type.
        $matches_proc[$i][0]=trim($matches_line[1]);

        // Trim whitespaces from the procedure name.
        $matches_proc[$i][1]=trim($matches_line[2]);

        // Trim whitespaces from the procedure arguments.
        $matches_proc[$i][2]=trim($matches_proc[$i][2]);

        // For custom data types, prepend "GL".
        $replaced_type=addDataTypePrefix($matches_proc[$i][0]);

        // If there is no lower case prefix, prepend "gl" to the name.
        $match=&$matches_proc[$i][1];
        if ($match[0]>='A' && $match[0]<='Z') {
            $match='gl'.$match;
        }

        // For custom data types, prepend "GL".
        $replaced_args=addDataTypePrefix($matches_proc[$i][2]);

        // Check that we have at least once prefixed an OpenGL data type (e.g.
        // the return type), else remove the invalid match.
        if (!$replaced_type && !$replaced_args) {
            // Note that $i is not an index but a key into the array. This means
            // deleting an entry will not make the following entries "move up".
            unset($matches_proc[$i]);
        }
    }

    $type_length_max=0;
    $name_length_max=0;
    $arguments_length_max=0;

    // Get the maximum string lengths for alignment.
    foreach ($matches_proc as $procedure) {
        list($type,$name,$arguments)=$procedure;

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

    $i=0;
    foreach ($matches_proc as $procedure) {
        if ($i++>0) {
            $contents.="\n";
        }

        list($type,$name,$arguments)=$procedure;

        // Write the padded function prototypes to a header file.
        $type_pad=str_pad($type,$type_length_max+1);
        $name_pad=str_pad($name,$name_length_max+1);

        $contents.=GLEX_PREFIX."PROC( $type_pad, $name_pad, ($arguments) );\n";

        $contents.="#ifndef $name\n";
        $contents.="    #define $name_pad".GLEX_PREFIX."$name\n";
        $contents.="#endif\n";
    }

    if (empty($contents)) {
        return $contents;
    }

    file_put_contents($file,$contents);

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

    $guard=strtoupper(strtr(basename($file),'.','_'));

    // Write the inclusion guard header.
    $contents.="#ifndef $guard\n";
    $contents.="#define $guard\n\n";

    $ignore=strtoupper($extension).'_IGNORE';

    $contents.="#ifdef __linux\n";
    $contents.="    // TODO: Linux support.\n";
    if (strpos($guard,"_WGL_")!==FALSE) {
        $contents.="\n    // Ignore WGL extensions under Linux.\n";
        $contents.="    #define $ignore\n";
    }
    $contents.="#endif\n\n";

    $contents.="#ifdef _WIN32\n";
    $contents.="    #ifndef WIN32_LEAN_AND_MEAN\n";
    $contents.="        #define WIN32_LEAN_AND_MEAN 1\n";
    $contents.="    #endif\n";
    $contents.="    #include <windows.h>\n";
    if (strpos($guard,"_GLX_")!==FALSE) {
        $contents.="\n    // Ignore GLX extensions under Windows.\n";
        $contents.="    #define $ignore\n";
    }
    $contents.="#endif\n\n";

    $contents.="#ifndef $ignore\n\n";

    $contents.="#include <GL/gl.h>\n";
    $contents.="#include \"GLEX_globals.h\"\n\n";

    extractTokensToString($tokens,$defines);
    if (!empty($defines)) {
        $contents.="// List the defined tokens introduced by this extension.\n";
        $contents.=$defines;
    }

    extractTypesToString($procs,$types);
    if (!empty($types)) {
        $contents.="// List the data types introduced by this extension.\n";
        $contents.=$types;
    }

    if (file_exists($extension.'_procs.h')) {
        $contents.="#ifdef __cplusplus\n";
        $contents.="extern \"C\"\n{\n";
        $contents.="#endif\n\n";

        $contents.='extern GLboolean '.$extension."_init(void);\n";
        $contents.="extern GLboolean $extension;\n\n";

        // Write the code that generates the prototype definitions.
        $contents.="// List the pointer prototypes for all functions of this extension.\n";
        $contents.='#define '.GLEX_PREFIX.'PROC(t,n,a) extern t (APIENTRY *'.GLEX_PREFIX."##n) a\n";
        $contents.='    #include "'.$extension.'_procs.h"'."\n";
        $contents.='#undef '.GLEX_PREFIX."PROC\n\n";

        $contents.="#ifdef __cplusplus\n";
        $contents.="} // extern \"C\"\n";
        $contents.="#endif\n\n";
    }

    $contents.="#endif // $ignore\n\n";

    // Write the inclusion guard footer.
    $contents.="#endif // $guard\n";

    if (empty($contents)) {
        return $contents;
    }

    file_put_contents($file,$contents);

    return $file;
}

function writeInitializationCode($extension) {
    global $cmdline;

    if (!file_exists($extension.'_procs.h')) {
        return $contents;
    }

    $file=$extension.'.c';
    if (!$cmdline) {
        $file=SERVER_TMP_DIRECTORY.$file;
    }

    $contents.='#include "'.$extension.'.h"'."\n\n";

    $ignore=strtoupper($extension).'_IGNORE';
    $contents.="#ifndef $ignore\n\n";

    // Write the code that generates the function pointer variables.
    $contents.="// Initialize all function pointers to 0.\n";
    $contents.='#define '.GLEX_PREFIX.'PROC(t,n,a) t (APIENTRY *'.GLEX_PREFIX."##n) a=0\n";
    $contents.='    #include "'.$extension.'_procs.h"'."\n";
    $contents.='#undef '.GLEX_PREFIX."PROC\n\n";

    $contents.="GLboolean $extension=GL_FALSE;\n\n";

    // Write the code that initializes the function pointer variables.
    $contents.="// Get the addresses for all functions of this extension.\n";
    $contents.='GLboolean '.$extension."_init(void)\n{\n";
    $contents.="    $extension=GL_TRUE;\n\n";

    $contents.='#define '.GLEX_PREFIX."PROC(t,n,a) $extension&=((*((PROC*)&".GLEX_PREFIX."##n)=wglGetProcAddress(#n))!=0)\n";
    $contents.='    #include "'.$extension.'_procs.h"'."\n";
    $contents.='#undef '.GLEX_PREFIX."PROC\n\n";

    $contents.="    return $extension;\n";
    $contents.="}\n\n";

    $contents.="#endif // $ignore\n";

    if (empty($contents)) {
        return $contents;
    }

    file_put_contents($file,$contents);

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
