<?php

function imagecolorgradient($img,$x,$y,$width,$height,$col_top,$col_bottom) {
    $steps=$height-1;
    $r_step=($col_bottom[0]-$col_top[0])/$steps;
    $g_step=($col_bottom[1]-$col_top[1])/$steps;
    $b_step=($col_bottom[2]-$col_top[2])/$steps;
    for ($i=0;$i<$height;++$i) {
        $r=$col_top[0]+round($i*$r_step);
        $g=$col_top[1]+round($i*$g_step);
        $b=$col_top[2]+round($i*$b_step);
        $col=imagecolorallocate($img,$r,$g,$b);
        imageline($img,$x,$y+$i,$x+$width-1,$y+$i,$col);
    }
}

// Although the manual sometimes skips this, it *is* necessary.
header("Content-Type: image/png");

$img=imagecreatetruecolor($width,$height);

// Parse the URL encoded colors into arrays.
$start=(int)($top[0]=='#');
$col_top=array(
    hexdec(substr($top,$start,2)),
    hexdec(substr($top,$start+2,2)),
    hexdec(substr($top,$start+4,2))
);

$start=(int)($bottom[0]=='#');
$col_bottom=array(
    hexdec(substr($bottom,$start,2)),
    hexdec(substr($bottom,$start+2,2)),
    hexdec(substr($bottom,$start+4,2))
);

// Create the vertical gradient.
imagecolorgradient($img,0,0,$width,$height,$col_top,$col_bottom);

imagepng($img);
imagedestroy($img);

?>
