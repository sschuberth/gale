<?= '<?xml version="1.0" encoding="iso-8859-1"?>' ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<html>

<head>
    <title>glex - OpenGL Extension Specification Parser</title>
    <style type="text/css">
        img { border-style: none; }

        .rounded { display: block; }
        .rounded * { display: block; height: 1px; overflow: hidden; background: #c7d1d2; }
        .margin_side_5 { margin: 0px 5px; }
        .margin_side_3 { margin: 0px 3px; }
        .margin_side_2 { margin: 0px 2px; }
        .margin_side_1 { margin: 0px 1px; height: 2px; }
        .margin_side_0 { margin: 0px; height: 6px; }

        hr.line { border-style: none; background-color: #abb5b5; color: #abb5b5; height: 1px; margin: 0px; }
    </style>
</head>

<body>
    <div class="rounded">
        <b class="margin_side_5"></b>
        <b class="margin_side_3"></b>
        <b class="margin_side_2"></b>
        <b class="margin_side_1"></b>
        <b class="margin_side_0"></b>
        <hr class="line" />
    </div>

    <p>
        Enter the URI or URL to OpenGL extension specification text file
        (see <a href="http://www.opengl.org/registry/">http://www.opengl.org/registry/</a>):
    </p>
    <form action="">
        <p><input type="file" size="85" accept="text/*" name="spec" /></p>
        <p><input type="button" value="Generate code" /></p>
    </form>
    <p>
        <a href="http://validator.w3.org/check?uri=referer">
            <img src="http://www.w3.org/Icons/valid-xhtml11" alt="Valid XHTML 1.1" width="88" height="31" />
        </a>
        <a href="http://jigsaw.w3.org/css-validator/check/referer">
            <img src="http://jigsaw.w3.org/css-validator/images/vcss" alt="Valid CSS" width="88" height="31" />
        </a>
    </p>
</body>

</html>
