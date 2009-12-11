<?php include 'site/site.php'; ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
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
	  <table cellspacing="0" cellpadding="0" align="right"><tr>
	    <?php echo(nav_button("main1", ""));?>
	    <?php echo(nav_button("main2", ""));?>
	    <?php echo(nav_button("main3", ""));?>
	    <?php echo(nav_button("main4", ""));?>
	    <?php echo(nav_button("main5", ""));?>
	    <?php echo(nav_button("main6", ""));?>
	    <?php echo(nav_button("main7", ""));?>
	    <?php echo(nav_button("main8", ""));?>
	    <?php echo(nav_button("docs", ""));?>
	  </tr></table>
        </td>
        <td id="header_last"></td>
      </tr></table>

     <div id="submenu">
       <?php nav_subs(); ?>
     </div>

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
	
