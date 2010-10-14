<?php header("Content-Type: application/xhtml+xml"); ?>
<?= '<?xml version="1.0" encoding="iso-8859-1"?>' ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">

<head>
    <title>glex - An OpenGL Extension Specification Parser (Version 2)</title>
    <link type="text/css" href="style.css" rel="stylesheet" />
</head>

<?php

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

<body>
    <table style="width: 50%">
        <?php drawTableBorder('t','white'); ?>
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="text-align: center; background-image: url(images/title.png)">
                <h2>glex, Version 2</h2>
                <h4>An OpenGL Extension Specification Parser</h4>
                <h6>(based on *.spec specification files)</h6>
            </td>
            <td><!-- Corner spacer --></td>
        </tr>
        <?php drawTableBorder('b','white'); ?>
    </table>

<?php

// GeSHi is included in any case for the version information in the footer.
include_once 'geshi/geshi.php';

if (empty($error)) {
    if (empty($es) || empty($fs) || empty($tm)) {
        echo '<div style="height: 128px"></div>';
    }
    else {
        $geshi=&new GeSHi();

        $geshi->enable_line_numbers(GESHI_NORMAL_LINE_NUMBERS);
        $geshi->set_overall_style('background-color: #c7d1d2; font-size: small;');
        $geshi->set_line_style('background: #d3e0e7;');
        $geshi->set_header_type(GESHI_HEADER_DIV);

        echo '<div style="height: 32px"></div>';

        // writeFunctionHeaderFile
        if (!empty($f)) {
            echo '<table style="width: 70%">';
                drawTableBorder('t','white',FALSE,2);
                showSourceCode($f);
                drawTableBorder('b','white',FALSE,2);
            echo '</table>';

            echo '<div style="height: 16px"></div>';
        }

        // writeEnumHeaderFile
        if (!empty($e)) {
            echo '<table style="width: 70%">';
                drawTableBorder('t','white',FALSE,2);
                showSourceCode($e);
                drawTableBorder('b','white',FALSE,2);
            echo '</table>';

            echo '<div style="height: 16px"></div>';
        }

        // writeInitializationCodeFile
        if (!empty($c)) {
            echo '<table style="width: 70%">';
                drawTableBorder('t','white',FALSE,2);
                showSourceCode($c);
                drawTableBorder('b','white',FALSE,2);
            echo '</table>';

            echo '<div style="height: 16px"></div>';
        }

        // GLEX_globals.h
        echo '<table style="width: 70%">';
            drawTableBorder('t','white',FALSE,2);
            showSourceCode('GLEX_globals.h');
            drawTableBorder('b','white',FALSE,2);
        echo '</table>';

        echo '<div style="height: 32px"></div>';
    }
}
else {
    echo '<div style="height: 32px"></div>';

    echo '<table style="width: 50%">';
        drawTableBorder('t','white');
        echo '
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="text-align: center">
                <h2>Error: '.$error.'</h2>
                <h4>Please make sure the <a href="'.$spec.'">specification file</a> is valid.</h4>
            </td>
            <td><!-- Corner spacer --></td>
        </tr>
        ';
        drawTableBorder('b','white');
    echo '</table>';

    echo '<div style="height: 32px"></div>';
}

?>

    <form action="glex.php">
        <table style="width: 50%">
            <?php drawTableBorder('t','#c7d1d2',TRUE); ?>
            <tr>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
                <td style="background-color: #d3e0e7; padding: 10px">
                    <?php $url='http://www.opengl.org/registry/api/enumext.spec'; ?>
                    Enter the URL to an OpenGL extension enumerants specification file
                    (e.g. <a href="<?= $url ?>"><?= $url ?></a>):
                    <p><input type="text" size="100%" name="es" value="<?= empty($es)?$url:$es ?>" /></p>
                </td>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            </tr>
            <tr>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
                <td style="background-color: #d3e0e7; padding: 10px">
                    <?php $url='http://www.opengl.org/registry/api/gl.spec'; ?>
                    Enter the URL to an OpenGL extension functions specification file
                    (e.g. <a href="<?= $url ?>"><?= $url ?></a>):
                    <p><input type="text" size="100%" name="fs" value="<?= empty($fs)?$url:$fs ?>" /></p>
                </td>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            </tr>
            <tr>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
                <td style="background-color: #d3e0e7; padding: 10px">
                    <?php $url='http://www.opengl.org/registry/api/gl.tm'; ?>
                    Enter the URL to an OpenGL typemap file
                    (e.g. <a href="<?= $url ?>"><?= $url ?></a>):
                    <p><input type="text" size="100%" name="tm" value="<?= empty($tm)?$url:$tm ?>" /></p>
                </td>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            </tr>
            <tr>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
                <td style="background-color: #d3e0e7; padding: 10px">
                    Enter the name of an API to generate code for
                    (e.g. "ARB_framebuffer_object" or "VERSION_2_0", optional):
                    <p><input type="text" size="100%" name="api" value="<?= empty($api)?'VERSION_2_0':$api ?>" /></p>
                </td>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            </tr>
            <tr>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
                <td style="background-color: #d3e0e7; padding: 10px; text-align: center">
                    <input type="submit" value="Generate code" />
                </td>
                <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            </tr>
            <?php drawTableBorder('b','#678285',TRUE); ?>
        </table>
    </form>

    <div style="height: 128px"></div>

    <table style="width: 50%">
        <?php drawTableBorder('t','white',FALSE,5); ?>
        <tr>
            <td colspan="7" style="text-align: center; font-size: 8pt">
                <a href="http://gale.berlios.de/glex2/">glex</a> is part of the <a href="http://developer.berlios.de/projects/gale/">gale project</a> generously hosted by <a href="http://www.berlios.de/">BerliOS</a>
                <br />&nbsp;
            </td>
        </tr>
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="width: 30%"><hr /></td>
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
            <td style="width: 30%"><hr /></td>
            <td><!-- Corner spacer --></td>
        </tr>
        <tr>
            <td colspan="7" style="text-align: center; font-size: 8pt">
                &nbsp;<br />
                Source code high-lighting is performed by <a href="http://qbnz.com/highlighter/">GeSHi</a> <?= GESHI_VERSION ?><br />
            </td>
        </tr>
        <?php drawTableBorder('b','white',FALSE,5); ?>
    </table>
</body>

</html>
