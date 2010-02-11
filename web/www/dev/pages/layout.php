<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN"
"http://www.w3.org/TR/html4/strict.dtd">

<html>
<head>
    <title>Enlightenment - <?php echo $name.' - '.$motto ?></title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">

    <link href="<?php url_style_image('favicon.png') ?>" rel="icon"  type="image/x-icon">
    <link href="<?php url_style_image('favicon.png') ?>" rel="shortcut icon" type="image/x-icon"  >
    <link href="<?php url_style_image('favicon.png') ?>" rel="icon" type="image/ico" >
    <link href="<?php url_style_image('favicon.png') ?>" rel="shortcut icon"  type="image/ico">

    <link rel="stylesheet" href="<?php url_css('print.css') ?>" type="text/css" media="print">
    <link rel="stylesheet" href="<?php url_css('screen.css') ?>" type="text/css" media="screen, projection">
    <link rel="stylesheet" href="<?php url_css($page .'.css') ?>" type="text/css" media="screen, projection">
    <!--[if lt IE 8]><link rel="stylesheet" href="<?php url_css('ie.css') ?>" type="text/css" media="screen, projection"><![endif]-->

    <script type="text/javascript">
        var gOverride = {
            urlBase: 'http://gridder.andreehansson.se/releases/latest/',
            gColor: '#00f',
            gColumns: 12,
            gOpacity: 0.10,
            gWidth: 9,
            pColor: '#00f',
            pHeight: 18,
            pOffset: 0,
            pOpacity: 0.15,
            center: true,
            gEnabled: true,
            pEnabled: true,
            setupEnabled: true,
            fixFlash: true,
            size: 960
        };
    </script>

</head>

<body class="<?php echo $page ?>">
<div id="everything">

    <div id="header">

       <div class="title">
            <h1><a href="<?php url_page('home') ?>"><span>Enlightenment</span></a></h1>
            <h2><span><?php echo $motto ?></span></h2>
       </div>

        <div class="menu">
            <ul>
            <?php
                foreach($header as $option)
                    if ( array_key_exists($option, $pages) )
                        echo "<li class='$option'><a href='".url_for($option)."'><span>".$pages[$option]."</span></a></li>\n";
            ?>
            </ul>
        </div>

        <?php if ( count(languages($page)) > 1): ?>
        <div class="localization-bar">
            <ul>
            <?php
                foreach ( languages($page) as $language)
                    echo "<li class='$language'><a href='".url_for('set_language/', $language, $page)."'>$language</a></li>";
             ?>
            </ul>
        </div>
        <?php endif ?>

        <?php if ( $notices = notices() ): ?>
        <div class="messages">
            <?php
                foreach ($notices as $notice)
                    if ( $notice['errno'] == 1024 )
                        echo "<p class='notice'>{$notice['errstr']}</p>\n";
                    elseif ( $notice['errno'] == 512 )
                        echo "<p class='warning'>{$notice['errstr']}</p>\n";
                    elseif (option('env') == ENV_DEVELOPMENT)
                        echo "<p class='error'>{$notice['errstr']}</p>\n";
            ?>
        </div>
        <?php endif ?>

    </div>

    <div id="content">
        <?php echo $content; ?>
    </div>

    <div id="footer">
        <div class="copyright">
            <p>©2009 The Enlightenment Project</p>
        </div>

        <div class="menu">
            <ul>
            <?php
                foreach($footer as $option)
                    if ( array_key_exists($option, $pages) )
                        echo "<li class='$option'><a href='".url_for($option)."'><span>".$pages[$option]."</span></a></li>\n";
            ?>
            </ul>
        </div>
    </div>

</div>
</body>
</html>

