<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN"
"http://www.w3.org/TR/html4/strict.dtd">

<html>
<head>
    <title>Enlightenment - Fatal website error (halt)</title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">

    <link href="<?php url_style_image('favicon.png') ?>" rel="icon"  type="image/x-icon">
    <link href="<?php url_style_image('favicon.png') ?>" rel="shortcut icon" type="image/x-icon"  >
    <link href="<?php url_style_image('favicon.png') ?>" rel="icon" type="image/ico" >
    <link href="<?php url_style_image('favicon.png') ?>" rel="shortcut icon"  type="image/ico">

    <link rel="stylesheet" href="<?php url_css('baseline.css') ?>" type="text/css">
    <link rel="stylesheet" href="<?php url_css('all-pages.css') ?>" type="text/css">
    <link rel="stylesheet" href="<?php url_css('error.css') ?>" type="text/css">
    <!--[if lt IE 8]> <link rel="stylesheet" href="<?php url_css('ie.css') ?>" type="text/css" media="screen, projection"> <![endif]-->

</head>

<body>
<div id="everything">

    <div id="header">
        <h1><a href="<?php  url_page('home') ?>"><span>Enlightenment</span></a></h1>
    </div>

    <div id="content">
        <?php echo $content; ?>
    </div>

    <div id="footer">
        <p id="copyright">©2009 The Enlightenment Project</p>
    </div>

</div>
</body>
</html>

