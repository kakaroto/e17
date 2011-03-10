<?php
  set_magic_quotes_runtime(0);

  if(isset($_GET['p']))
	$page = $_GET['p'];
  else
    $page = '';

  if(isset($_GET['l']))
    $lang = $_GET['l'];
  else
	$lang = 'en';

  $page = str_replace(".", "", $page);
  if ($page == "") $page = "index";
  if ($page[0] == "/") $page="index";

  $lang = str_replace(".", "", $lang);
  $lang = str_replace("/", "", $lang);
  if ($lang == "") $lang = "en";

  if (!file_exists("p/$page")) $page="index";
  if (!file_exists("p/$page/$lang-body")) $lang="en";

  $title = read_var("p/$page/$lang-title");

/* Generic Table printing helpers */

function td($content)
{
    if ( empty($content) )
        echo '&nbsp;';
    else
        echo $content;
}

function tr_odd($reset = false)
{
    static $n = 0;

    if ( $reset )
        $n = 0;
    else if ($n++ & 1)
        echo 'class="odd"';
}

/* Developer Data printing helpers */

function dev_data_photo($data)
{
    if ( $data['Photo'] )
        return "<img src='http://download.enlightenment.org/devs/{$data['Login']}/icon-sml.png' width='40' height='54' border='0'>";
    else
        return "<img src='i/logo-s.png' width='18' height='24' border='0'>";
}

function dev_data_www($data)
{
    $url = @$data["WWW"];

    if ( strstr($url, 'http://') )
    {
        $domain = str_replace(array('http://', 'www.'),'', $url);
        $domain = rtrim($domain, '/');
        return "<a href='$url'>$domain</a>";
    }

    return null;
}

function dev_data_contact($data)
{
    $name = @$data['Name'];
    $mail = @$data['E-Mail'];

    if ( !empty($mail) )
    {
       // Devs whose e-mail begins with a '-' prefer it not be on the
       //  contact page, in an effort to minimize spam.
       if ( substr($mail, 0, 1) != "-" )
          return "<a href='mailto:$mail'>$name</a>";
    }

    return $name;
}

function dev_data_irc($data)
{
    $nick = @$data['IRC Nick'];

    if ( $nick )
    {
        $nick = str_replace(' ', '', $nick);
        return str_replace('/', ', ', $nick);
    }

    return null;
}

/* replacements for usual <img> and <a href> etc, tags that fill in the
 * src, dest, thumb etc. for you */
  function img($src, $alt) {
      global $page;
      global $lang;
      list($width, $height, $type, $attr) = getimagesize("p/$page/d/$src");
      echo("<img src=\"p/$page/d/$src\" style='width:".$width."px; height:".$height."px; border:0;' alt='$alt'>");
  }

  function thm($src, $alt) {
      global $page;
      global $lang;
      list($width, $height, $type, $attr) = getimagesize("p/$page/d/.t.$src");
      echo("<a href='p/$page/d/$src'><img src='p/$page/d/.t.$src' style='width:".$width."px; height:".$height."px;' alt='$alt' class='thumb'/></a>");
  }

  function a($pg, $txt) {
      global $page;
      global $lang;
      echo("<a href='p.php?p=$pg&amp;l=$lang'>$txt</a>");
  }

  function ast($pg) {
      global $page;
      global $lang;
      echo("<a href='p.php?p=$pg&amp;l=$lang'>");
  }

  function acl($pg, $cl) {
      global $page;
      global $lang;
      echo("<a href='p.php?p=$pg&amp;l=$lang' class='$cl'>");
  }

  function blink($ic, $txt, $pg) {
      echo("<table class='b' cellpadding='0' border='0' cellspacing='0'>");
      echo("<tr><td class='b1'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b2'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b3'><img src='i/_.gif' width='8' height='8'></td></tr>");
      echo("<tr><td class='b4'><img src='i/_.gif' width='8' height='24'></td>");
      echo("<td class='b5'>");
      acl($pg, "b");
      echo("<img src='i/_.gif' width='8' height='24'>");
      list($width, $height, $type, $attr) = getimagesize("$ic");
      echo("<img src='$ic' width='$width' height='$height'>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      echo("<b>$txt</b>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      echo("</a>");
      echo("</td>");
      echo("<td class='b6'><img src='i/_.gif' width='8' height='24'></td></tr>");
      echo("<tr><td class='b7'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b8'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b9'><img src='i/_.gif' width='8' height='8'></td></tr>");
      echo("</table>");
  }

  function blinkdl($ic, $txt, $url) {
      echo("<table class='b' cellpadding='0' border='0' cellspacing='0'>");
      echo("<tr><td class='b1'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b2' width='100%'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b3'><img src='i/_.gif' width='8' height='8'></td></tr>");
      echo("<tr><td class='b4'><img src='i/_.gif' width='8' height='24'></td>");
      echo("<td class='b5' width='100%'>");
      echo("<a href='$url' class='b'>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      list($width, $height, $type, $attr) = getimagesize("$ic");
      echo("<img src='$ic' width='$width' height='$height'>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      echo("<b>$txt</b>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      echo("</a>");
      echo("</td>");
      echo("<td class='b6'><img src='i/_.gif' width='8' height='24'></td></tr>");
      echo("<tr><td class='b7'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b8' width='100%'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b9'><img src='i/_.gif' width='8' height='8'></td></tr>");
      echo("</table>");
  }

  function blinkurl($ic, $txt, $url) {
      echo("<table class='b' cellpadding='0' width='100%' border='0' cellspacing='0'>");
      echo("<tr><td class='b1'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b2' width='100%'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b3'><img src='i/_.gif' width='8' height='8'></td></tr>");
      echo("<tr><td class='b4'><img src='i/_.gif' width='8' height='24'></td>");
      echo("<td class='b5' width='100%'>");
      echo("<a href='$url' class='b'>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      list($width, $height, $type, $attr) = getimagesize("$ic");
      echo("<img src='$ic' width='$width' height='$height'>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      echo("<b>$txt</b>");
      echo("<img src='i/_.gif' width='8' height='24'>");
      echo("</a>");
      echo("</td>");
      echo("<td class='b6'><img src='i/_.gif' width='8' height='24'></td></tr>");
      echo("<tr><td class='b7'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b8' width='100%'><img src='i/_.gif' width='8' height='8'></td>");
      echo("<td class='b9'><img src='i/_.gif' width='8' height='8'></td></tr>");
      echo("</table>");
  }

  function frame1($w) {
      echo("<table class='pn' cellpadding='0' border='0' cellspacing='0' $w>");
      echo("<tr><td class='pn1' width=24><img src='i/_.gif' width='24' height='24'></td>");
      echo("<td class='pn2' width='100%'><img src='i/_.gif' width='24' height='24'></td>");
      echo("<td class='pn3' width=24><img src='i/_.gif' width='24' height='24'></td></tr>");
      echo("<tr><td class='pn4' width=24><img src='i/_.gif' width='24' height='24'></td>");
      echo("<td class='pn5' width='100%'>");
  }
  function frame2() {
      echo("</td>");
      echo("<td class='pn6' width=24><img src='i/_.gif' width='24' height='24'></td></tr>");
      echo("<tr><td class='pn7' width=24><img src='i/_.gif' width='24' height='24'></td>");
      echo("<td class='pn8' width='100%'><img src='i/_.gif' width='24' height='24'></td>");
      echo("<td class='pn9' width=24><img src='i/_.gif' width='24' height='24'></td></tr>");
      echo("</table>");
  }

/* utility functions */
  function read_var($f) {
      if (file_exists($f)) {
          $handle = fopen($f, "r");
	  if ($handle) {
	      $fl = fgets($handle, 4096);
	      $fl = str_replace("\n", "", $fl);
	      return $fl;
	  }
      }
      return "";
  }

/* functions to output sections of the page */
  function show_langs($f) {
      global $page;
      global $lang;
      if (file_exists($f)) {
	  $handle = fopen($f, "r");
	  if ($handle) {
	      while (!feof($handle)) {
		  $fl = fgets($handle, 4096);
		  $fl = str_replace("\n", "", $fl);
		  if ($fl != "") {
		      echo("<a href='p.php?p=$page&amp;l=$fl'><img src='i/$fl.png' style='width:24px; height:18px; border:0;' alt='$fl'/></a>\n");
		  }
	      }
	  }
      }
  }

  function nav_button($b) {
      global $page;
      global $lang;
      $c = "passive";

      if (file_exists("p/$b/$lang-label"))
      $l = read_var("p/$b/$lang-label");
      else
      $l = read_var("p/$b/en-label");
      if (file_exists("p/$b/page")) {
	  $h = read_var("p/$b/page");
	  $pname = $h;
	  $h = "p.php?p=$h&amp;l=$lang";
      }
      else if (file_exists("p/$b/link")) {
	  $h = read_var("p/$b/link");
	  $pname = $h;
      }
      if ((strncasecmp($page,$l,strlen($l)) == 0) OR
          (($page == "index") AND ($l == "Home") )) {
      	  $c = "active";
          return "<li class='$pname current'><a><span>$l</span></a></li>\n";
      }
      return "<li class='$pname'><a href='$h'><span>$l</span></a></li>\n";
  }

  function nav_subs() {
      global $page;
      global $lang;
      $item = 0;
      $arr = explode("/", $page);
      $p = "";
      for ($i=0; $i<count($arr); $i++) {
	   if ($p != "") $p .= "/";
           $p .= $arr[$i];

      if (file_exists("p/$p/subs")) {
	  $handle = fopen("p/$p/subs", "r");
	  if ($handle) {
	      $item = 0;
	      while (!feof($handle)) {
		  $fl = fgets($handle, 4096);
		  $fl = str_replace("\n", "", $fl);
		  if ($fl != "") {
		      $h = "";
		      if (file_exists("p/$p/$fl/link")) {
			  $h = read_var("p/$p/$fl/link");
		      }
		      $class="";
		      if ( "$p/$fl" == $page ) {
                          $class = " selected";
		      }

		      if (file_exists("p/$p/$fl/$lang-label")) {
			  $l = read_var("p/$p/$fl/$lang-label");
			  if ($h == "") {
			    $h = "p.php?p=$p/$fl&amp;l=$lang";
                          }
			  echo("<li class='$class'><a href='$h'><span>$l</span></a></li>\n");
			  $item++;
		      }
		      else if (file_exists("p/$p/$fl/en-label")) {
			  $l = read_var("p/$p/$fl/en-label");
			  if ($h == "") {
			    $h = "p.php?p=$p/$fl&amp;l=$lang";
                          }
			  echo("<li class='$class'><a href='$h'><span>$l</span></a></li>\n");
			  $item++;
		      }
		  }
	      }
	  }
      }
      }
  }

/* Don't add the ?> */
