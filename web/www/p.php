<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN"
"http://www.w3.org/TR/html4/strict.dtd">
<?php
    include 'site/site.php';
    include 'site/developers.php';
?>
<html>
<head>
<title><?php echo $title; ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css">
<link rel="stylesheet" type="text/css" href="theme/css/default.css">
<!--[if lt IE 8]> <link rel="stylesheet" type="text/css" href="theme/css/ie-lt8.css"> <![endif]-->
<link rel="icon" href="favicon.png" type="image/x-icon">
<link rel="shortcut icon" href="favicon.png" type="image/x-icon">
<link rel="icon" href="favicon.png" type="image/ico">
<link rel="shortcut icon" href="favicon.png" type="image/ico">
<?php

if (is_file("p/$page/$lang-head")) include "p/$page/$lang-head";

if (is_file("p/$page/$lang-rss"))
   echo '<link rel="alternate" type="application/rss+xml" title="RSS" href="rss.php?p=$page&l=$lang">';

 ?>
<script type="text/javascript">
/* <![CDATA[ */
    (function() {
        var s = document.createElement('script'), t = document.getElementsByTagName('script')[0];
        s.type = 'text/javascript';
        s.async = true;
        s.src = 'http://api.flattr.com/js/0.6/load.js?mode=auto';
        t.parentNode.insertBefore(s, t);
    })();
/* ]]> */</script>
</head>

<body>

<div id="wrapper">

    <div id="header">

        <div class="menu">
        <div class="layout">

            <ul class="menu">
                <?php echo(nav_button("homelink", ""));?>
                <?php echo(nav_button("news", ""));?>
                <?php echo(nav_button("about", ""));?>
                <?php echo(nav_button("download", ""));?>
                <?php echo(nav_button("support", ""));?>
                <?php echo(nav_button("contribute", ""));?>
                <?php echo(nav_button("contact", ""));?>
                <?php echo(nav_button("docs", ""));?>
                <li class="tracker"><a href="http://trac.enlightenment.org/e"><span>Tracker</span></a></li>
		<li class="events"><a href="http://trac.enlightenment.org/e/wiki/Events"><span>Events</span></a></li>
            </ul>
        </div>
        </div>

        <div class="submenu">
        <div class="layout">

            <ul class="submenu">
                <?php nav_subs(); ?>
                <li class=''><?php a("donate", "<span>Donate</span>");?></a></li>
		<li><span><a class="FlattrButton" style="display:none;" rev="flattr;button:compact;" href="https://flattr.com/profile/enlightenment"></a><noscript><a href="http://flattr.com/thing/418692/Enlightenment" target="_blank"><img src="http://api.flattr.com/button/flattr-badge-large.png" alt="Flattr this" title="Flattr this" border="0" /></a></noscript></span></li>
                <li class=''><a href=http://twitter.com/_Enlightenment_><span><img src="http://www.enlightenment.org/i/soc-i-tw.png" border=0 height=16 width=16></span></a></li>
                <li class=''><a href=http://plus.google.com/118426816251488376359><span><img src="http://www.enlightenment.org/i/soc-i-g+.png" border=0 height=16 width=16></span></a></li>
                <li class=''><a href=http://www.facebook.com/enlightenment.org><span><img src="http://www.enlightenment.org/i/soc-i-fb.png" border=0 height=16 width=16></span></a></li>
                <li class=''><a href=http://planet.enlightenment.org/><span><img src="http://www.enlightenment.org/i/feed-icon-14x14.png" border=0 height=16 width=16></span></a></li>
            </ul>

        </div>
        </div>

    </div>

    <div id="middle">
    <div class="layout">

        <div id="content">
            <?php include "p/$page/$lang-body" ?>
        </div>

    </div>
    </div>

    <div id="push"></div>

</div>

<div id="sitefooter">
<div class="layout">

  <p>
Copyright &copy; Enlightenment.org
  </p>

</div>
</div>

</body>
</html>

