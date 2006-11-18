<?= '<?xml version="1.0" encoding="iso-8859-1"?>' ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">

<head>
    <title>glex - An OpenGL Extension Specification Parser</title>
    <link type="text/css" href="style.css" rel="stylesheet" />
</head>

<body>
    <table style="width: 50%">
        <tr>
            <td class="corner"><img src="images/corner-tl-white.png" alt="" /></td>
            <td style="width: 100%"></td>
            <td class="corner"><img src="images/corner-tr-white.png" alt="" /></td>
        </tr>
        <tr>
            <td><!-- Corner spacer --></td>
            <td style="text-align: center">
                <h2>glex</h2>
                <h4>An OpenGL Extension Specification Parser</h4>
            </td>
            <td><!-- Corner spacer --></td>
        </tr>
        <tr>
            <td class="corner"><img src="images/corner-bl-white.png" alt="" /></td>
            <td style="width: 100%"></td>
            <td class="corner"><img src="images/corner-br-white.png" alt="" /></td>
        </tr>
    </table>

    <div style="height: 128px"></div>

    <table style="width: 70%">
        <tr>
            <td class="corner"><img src="images/corner-tl.png" alt="" /></td>
            <td style="width: 100%; background: url(images/edge-t.png) repeat-x"></td>
            <td class="corner"><img src="images/corner-tr.png" alt="" /></td>
        </tr>
        <tr>
            <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
            <td style="background-color: #d3e0e7; padding: 10px">
                Enter the URI or URL to OpenGL extension specification text file
                (see e.g. <a href="http://www.opengl.org/registry/">http://www.opengl.org/registry/</a>):
                <form action="parse.php">
                    <p>
                        <input type="text" size="100%" name="spec" />
                        <input type="submit" value="Generate code" />
                    </p>
                </form>
            </td>
            <td style="background-color: #d3e0e7"><!-- Corner spacer --></td>
        </tr>
        <tr>
            <td class="corner"><img src="images/corner-bl.png" alt="" /></td>
            <td style="width: 100%; background: url(images/edge-b.png) repeat-x"></td>
            <td class="corner"><img src="images/corner-br.png" alt="" /></td>
        </tr>
    </table>

    <div style="height: 128px"></div>

    <table style="width: 50%">
        <tr>
            <td class="corner"><img src="images/corner-tl-white.png" alt="" /></td>
            <td colspan="5" style="width: 100%"></td>
            <td class="corner"><img src="images/corner-tr-white.png" alt="" /></td>
        </tr>
        <tr>
            <td colspan="7" style="text-align: center; font-size: 8pt">
                <a href="http://gale.berlios.de/glex/">glex</a> is part of the <a href="http://developer.berlios.de/projects/gale/">gale project</a> hostet at <a href="http://www.berlios.de/">BerliOS</a><br />
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
        <tr>
            <td class="corner"><img src="images/corner-bl-white.png" alt="" /></td>
            <td colspan="5" style="width: 100%"></td>
            <td class="corner"><img src="images/corner-br-white.png" alt="" /></td>
        </tr>
    </table>
</body>

</html>
