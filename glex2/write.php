<?php

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

?>
