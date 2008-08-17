<?php include 'site/site.php'; ?>
<html>
<head>
<title><?php echo $title; ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<link rel="stylesheet" type="text/css" href="style.css"/>
<link rel="icon" href="favicon.png" type="image/x-icon"/>
<link rel="shortcut icon" href="favicon.png" type="image/x-icon"/>
<link rel="icon" href="favicon.png" type="image/ico"/>
<link rel="shortcut icon" href="favicon.png" type="image/ico"/>
<style type="text/css"><!--img {behavior: url("png.htc");}--></style>
<script src="bgsleight.js" type="text/javascript"></script>
</head>

<body>
<table class='h'>
  <tr>
    <td class="t">
     <table class='n nl' cellpadding='0' cellspacing='0'>
      <?php
        echo(nav_button("main1", "lnav"));
        echo(nav_button("main2", "lnav"));
        echo(nav_button("main3", "lnav"));
        echo(nav_button("main4", "lnav"));
      ?>
     </table>
    <div class='l ll'><?php show_langs("p/lang1"); ?></div>
    </td>
    <td class="tl">
     <img src="i/_.gif" style='width:600px; height:1px;' alt=' '/><br/>
     <?php nav_subs(); ?>
    </td>
    <td class="t">
     <table class='n nr' cellpadding='0' cellspacing='0'>
      <?php
        echo(nav_button("main5", "rnav"));
        echo(nav_button("main6", "rnav"));
        echo(nav_button("main7", "rnav"));
        echo(nav_button("main8", "rnav"));
      ?>
     </table>
     <div class='l lr'><?php show_langs("p/lang2"); ?></div>  
    </td>
  </tr>
</table>
<div class="content">
<div class="main">	
<?php include "p/$page/$lang-body" ?>
</div>
</div>
<div class="footer">
<hr/>
<p class="tiny">Copyright &copy; Enlightenment.org</p>
</div>
</body>
</html>
	
