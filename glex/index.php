<?= '<?xml version="1.0" encoding="iso-8859-1"?>' ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">

<head>
    <title>glex - An OpenGL Extension Specification Parser</title>
    <link type="text/css" href="style.css" rel="stylesheet" />
</head>

<?php

function drawTableBorder($prefix,$color,$edge=false,$colspan=1) {
    $color_url=urlencode($color);

    echo '<tr>';
    echo '<td class="corner"><img src="images/corner-'.$prefix.'l-'.$color_url.'.png" alt="" /></td>';

    echo '<td colspan="'.$colspan.'" style="width: 100%; background-color: '.$color;
    if ($edge)
        echo '; background-image: url(images/edge-'.$prefix.'.png); background-repeat: repeat-x';
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
        <td><a href="download.php?file='.$f.'">'.$f.'</a></td>
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

<body>
    <table style="width: 50%">
        <?php drawTableBorder('t','white'); ?>
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="text-align: center">
                <h2>glex</h2>
                <h4>An OpenGL Extension Specification Parser</h4>
            </td>
            <td><!-- Corner spacer --></td>
        </tr>
        <?php drawTableBorder('b','white'); ?>
    </table>

<?php

if (empty($spec)) {
    echo '<div style="height: 128px"></div>';
} else {
    include_once 'geshi/geshi.php';

    $geshi=&new GeSHi();

    $geshi->enable_line_numbers(GESHI_NORMAL_LINE_NUMBERS);
    $geshi->set_overall_style('background-color: #c7d1d2;');
    $geshi->set_line_style('background: #d3e0e7;');
    $geshi->set_header_type(GESHI_HEADER_DIV);

    echo '<div style="height: 32px"></div>';

    // writeMacroHeader
    echo '<table style="width: 70%">';
        drawTableBorder('t','white',false,2);
        showSourceCode($p);
        drawTableBorder('b','white',false,2);
    echo '</table>';

    echo '<div style="height: 16px"></div>';

    // writePrototypeHeader
    echo '<table style="width: 70%">';
        drawTableBorder('t','white',false,2);
        showSourceCode($h);
        drawTableBorder('b','white',false,2);
    echo '</table>';

    echo '<div style="height: 16px"></div>';

    // writeInitializationCode
    echo '<table style="width: 70%">';
        drawTableBorder('t','white',false,2);
        showSourceCode($c);
        drawTableBorder('b','white',false,2);
    echo '</table>';

    echo '<div style="height: 32px"></div>';
}

?>

    <table style="width: 70%">
        <?php drawTableBorder('t','#c7d1d2',true); ?>
        <tr>
            <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            <td style="background-color: #d3e0e7; padding: 10px">
                Enter the URI or URL to OpenGL extension specification text file
                (see e.g. <a href="http://www.opengl.org/registry/">http://www.opengl.org/registry/</a>):
                <form action="parse.php">
                    <p>
                        <input type="text" size="100%" name="spec" value="<?= $spec ?>" />
                        <input type="submit" value="Generate code" />
                    </p>
                </form>
            </td>
            <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
        </tr>
        <?php drawTableBorder('b','#678285',true); ?>
    </table>

    <div style="height: 128px"></div>

    <table style="width: 50%">
        <?php drawTableBorder('t','white',false,5); ?>
        <tr>
            <td colspan="7" style="text-align: center; font-size: 8pt">
                <a href="http://gale.berlios.de/glex/">glex</a> is part of the <a href="http://developer.berlios.de/projects/gale/">gale project</a> generously hosted by <a href="http://www.berlios.de/">BerliOS</a><br />
                Source code high-lighting is performed by <a href="http://qbnz.com/highlighter/">GeSHi</a><br />
                &nbsp;
            </td>
        </tr>
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="width: 35%"><hr /></td>
            <td style="text-align: center">
                <a href="http://validator.w3.org/check?uri=referer">
                    <img style="display: inline" src="images/valid-xhtml11.png" alt="Valid XHTML 1.1" />
                </a>
            </td>
            <td style="width: 5%"><hr /></td>
            <td style="text-align: center">
                <a href="http://jigsaw.w3.org/css-validator/check/referer">
                    <img style="display: inline" src="images/valid-css.png" alt="Valid CSS" />
                </a>
            </td>
            <td style="width: 35%"><hr /></td>
            <td><!-- Corner spacer --></td>
        </tr>
        <?php drawTableBorder('b','white',false,5); ?>
    </table>
</body>

</html>
