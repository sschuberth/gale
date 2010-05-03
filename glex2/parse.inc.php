<?php

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
    if (preg_match('/^(\w+) enum:$/',$line,$matches)) {
        $table[$matches[1]]=array();
        end($table);
    }
    // Example:
    // 	DEPTH_BUFFER_BIT				= 0x00000100	# AttribMask
    else if (preg_match('/^(\w+)\s*=\s*(\w+)$/',$line,$matches)) {
        $table[key($table)]['GL_'.$matches[1]]=$matches[2];
    }
    // Example:
    // 	use ARB_depth_buffer_float	    DEPTH_COMPONENT32F
    else if (preg_match('/^use\s+(\w+)\s+(\w+)$/',$line,$matches)) {
        $table[key($table)]['GL_'.$matches[2]]=$matches[1];
    }
}

function parseEnumSpec($file,&$table) {
    global $debug;

    if ($debug>=1) {
        echo "*** DEBUG *** Entering parseEnumSpec().\n";
        echo "*** DEBUG *** Parsing file \"$file\".\n";
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
        $res_value=$defines[$key];
        if (!empty($res_value)) {
            $value=$res_value;
            return;
        }

        if ($debug>=1) {
            echo "*** DEBUG *** Unable to resolve \"$key\" in \"$value\".\n";
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

        $res_value=$defines[$key2];
        if (!empty($res_value)) {
            $value=$res_value;
            return;
        }

        // In case the extension name is wrong, search the whole table.
        foreach ($table as $defines) {
            $res_value=$defines[$key];
            if (is_numeric($res_value)) {
                $value=$res_value;
                return;
            }

            $res_value=$defines[$key2];
            if (is_numeric($res_value)) {
                $value=$res_value;
                return;
            }
        }

        // We did not find a value for this enum.
        $value='NULL';

        if ($debug>=1) {
            echo "*** DEBUG *** Ultimately did not find \"$key\".\n";
        }
    }

    array_walk_recursive($table,'resolveUseDirectives',$table);

    if ($debug>=2) {
        var_dump($table);
    }

    if ($debug>=1) {
        echo "*** DEBUG *** Leaving parseEnumSpec().\n";
    }
}

function callbackFuncSpec($line,&$fs_table) {
    global $tm_table;

    // Examples:
    // required-props:
    // param:		retval retained
    // passthru: #include <stddef.h>
    if (preg_match('/^[\w-]+:/',$line)) {
        return;
    }

    // Example:
    // CullFace(mode)
    if (preg_match('/^(\w+)\(.*\)$/',$line,$matches)) {
        $fs_table['gl'.$matches[1]]=array();
        end($fs_table);
    }
    // Examples:
    // 	return		UInt32
    // 	param		mask		ClearBufferMask in value
    // 	category	VERSION_1_0		   # old: framebuf
    else if (preg_match('/^(\w+)\s+(\w+)\s*(\w*)[\s\w]*$/',$line,$matches)) {
        $entry=&$fs_table[key($fs_table)][$matches[1]];
        if (!empty($entry)) {
            // Separate multiple entries per value by commas.
            $entry.=', ';
        }
        if (empty($matches[3])) {
            $type=$tm_table[$matches[2]];
            if (!empty($type) && $matches[1]=='return') {
                // Translate the return type.
                $entry.=$type;
            }
            else {
                $entry.=$matches[2];
            }
        }
        else {
            // Translate the argument type and append the argument name.
            $entry.=$tm_table[$matches[3]].' '.$matches[2];
        }
    }
}

function parseFuncSpec($file,&$table) {
    global $debug;

    if ($debug>=1) {
        echo "*** DEBUG *** Entering parseFuncSpec().\n";
        echo "*** DEBUG *** Parsing file \"$file\".\n";
    }

    parseFile('callbackFuncSpec',$file,$tmp_table);
    reset($tmp_table);

    // Create the final table using categories as keys and complete function
    // prototypes as values.
    foreach ($tmp_table as $func => $props) {
        $proto=$props['return'].' '.$func.'('.$props['param'].');';
        $table[$props["category"]][]=$proto;
    }

    if ($debug>=2) {
        var_dump($table);
    }

    if ($debug>=1) {
        echo "*** DEBUG *** Leaving parseFuncSpec().\n";
    }
}

function callbackTypeMap($line,&$table) {
    // Examples:
    // AccumOp,*,*,			    GLenum,*,*
    // void,*,*,			    *,*,*
    $line=str_replace(array(" ","\t"),array("",""),$line);
    $entry=explode(',',$line);
    if ($entry[3]!='*') {
        $table[$entry[0]]=$entry[3];
    }
}

function parseTypeMap($file,&$table) {
    global $debug;

    if ($debug>=1) {
        echo "*** DEBUG *** Entering parseTypeMap().\n";
        echo "*** DEBUG *** Parsing file \"$file\".\n";
    }

    parseFile('callbackTypeMap',$file,$table);

    if ($debug>=2) {
        var_dump($table);
    }

    if ($debug>=1) {
        echo "*** DEBUG *** Leaving parseTypeMap().\n";
    }
}

?>
