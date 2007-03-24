<?php include_once 'site/site.php'; ?>
<!doctype html public "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title><?php print($title); ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css">
<link rel="icon" href="favicon.ico" type="image/x-icon">
<link rel="shortcut icon" href="favicon.ico" type="image/x-icon">
<link rel="icon" href="favicon.ico" type="image/ico">
<link rel="shortcut icon" href="favicon.ico" type="image/ico">
<style type="text/css">img {behavior: url("png.htc");} </style>
<script src="bgsleight.js" type="text/javascript"></script>
</head>

<body>
<table width=100% border=0 cellpadding=0 cellspacing=0><tr>
<td class="t"  width=50% valign=top align=left>
 <table width=100px height=100px border=0 cellpadding=0 cellspacing=0>
  <?php
    nav_button("main1", "lnav");
    nav_button("main2", "lnav");
    nav_button("main3", "lnav");
    nav_button("main4", "lnav");
  ?>
 </table>
<img src="i/_.gif" width=1 height=8><br>
<img src="i/_.gif" width=4 height=4>
  <?php show_langs("p/lang1"); ?>
</td>
<td class="tl" width=612 height=250 valign=bottom align=center>
 <img src="i/_.gif" width=612 height=1><?php nav_subs(); ?>
</td>
<td class="t"  width=50% valign=top align=right>
 <table width=100px height=100px border=0 cellpadding=0 cellspacing=0>
  <?php
    nav_button("main5", "rnav");
    nav_button("main6", "rnav");
    nav_button("main7", "rnav");
    nav_button("main8", "rnav");
  ?>
 </table>
<img src="i/_.gif" width=1 height=8><br>
  <?php show_langs("p/lang2"); ?>
<img src="i/_.gif" width=4 height=4>
</td>
</tr></table>

<center>
<div class="bod"><?php include_once "p/$page/$lang-body" ?></div>
</center>

<hr>
<p class="tiny">Copyright &copy; Enlightenment.org</p>
</body>
</html>
	
