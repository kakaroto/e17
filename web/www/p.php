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
</head>

<body>

  <div id="wrapper">
    <div id="layout">



      <!-- Custom Header -->
      <table cellspacing="0" cellpadding="0" width="100%"><tr>
        <td id="header_logo">
          <a href="http://www.enlightenment.org"></a>
        </td>
        <td id="header_menu">
          <ul>
	    <?php echo(nav_button("main8", ""));?>
	    <?php echo(nav_button("main7", ""));?>
	    <?php echo(nav_button("main6", ""));?>
	    <?php echo(nav_button("main5", ""));?>
	    <?php echo(nav_button("main4", ""));?>
	    <?php echo(nav_button("main3", ""));?>
	    <?php echo(nav_button("main2", ""));?>
	    <?php echo(nav_button("main1", ""));?>
          </ul>
        </td>
        <td id="header_last"></td>
      </tr></table>

     <div id="submenu">
       <?php nav_subs(); ?>
     </div>
<!--<table class='h' border=1>
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
</table>-->

      <!-- CM contents -->
      <div id="content">
        <?php include "p/$page/$lang-body" ?>
      </div>
      
      <div id="push"></div>
    </div><!-- /layout -->
  </div><!-- /wrapper -->


  <!-- Custom Footer -->
  <div id="sitefooter">
    <table width="100%"><tr>
      <td width="100%" align="center">Copyright &copy; Enlightenment.org</td>
    </tr></table>
  </div><!-- /sitefooter -->


</body>
</html>
	
