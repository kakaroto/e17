<?php include 'site/site.php'; ?>
<html>
<head>
<title><?php echo $title; ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css">
<link rel="stylesheet" type="text/css" href="/theme/css/screen.css">
<!--[if lt IE 7]> <link rel="stylesheet" type="text/css" href="/theme/ie/ie6.css"> <![endif]-->
<link rel="icon" href="favicon.png" type="image/x-icon">
<link rel="shortcut icon" href="favicon.png" type="image/x-icon">
<link rel="icon" href="favicon.png" type="image/ico">
<link rel="shortcut icon" href="favicon.png" type="image/ico">
<?php

if (is_file("p/$page/$lang-head")) include "p/$page/$lang-head";

if (is_file("p/$page/$lang-rss"))
   echo '<link rel="alternate" type="application/rss+xml" title="RSS" href="rss.php?p=$page&l=$lang">';

 ?>
</head>

<body>

<div id="wrapper">

    <div id="header">

        <div class="menu">
        <div class="layout">

            <?php $class = "logo"; if ($page == "index") $class = "logo current" ?>
            <h1 class="<?php echo $class ?>">
                <a href="/" title="Homepage" name="Homepage"><span>Enlightenment.org</span></a>
            </h1>

            <ul class="menu">
                <?php echo(nav_button("main2", ""));?>
                <?php echo(nav_button("main3", ""));?>
                <?php echo(nav_button("main4", ""));?>
                <?php echo(nav_button("main5", ""));?>
                <?php echo(nav_button("main6", ""));?>
                <?php echo(nav_button("main7", ""));?>
                <?php echo(nav_button("docs", ""));?>
                <?php echo(nav_button("main8", ""));?>
            </ul>

        </div>
        </div>

        <div class="submenu">
        <div class="layout">

            <?php nav_subs(); ?>

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

    <table width="100%">
        <tr>
            <td width="100%" align="center">Copyright &copy; Enlightenment.org</td>
        </tr>
    </table>

</div>
</div>

</body>
</html>

