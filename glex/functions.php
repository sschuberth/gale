<?php

require_once 'constants.php';

function parseSpecIntoArray($spec,&$struct) {
    // Parse the sections and contents of the file into a structure array.
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

        $ltline=ltrim($line);
        if (empty($line) || $line!=$ltline) {
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
    $section=NULL;
    $content=NULL;
    fclose($handle);
}

?>
