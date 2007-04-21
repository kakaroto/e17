<?php
  set_magic_quotes_runtime(0);

  if(isset($_GET['p']))
	$page = $_GET['p'];
  else
    $page = '';

  if(isset($_GET['l']))
    $lang = $_GET['l'];
  else
	$lang = '';

  $page = str_replace(".", "", $page);
  if ($page == "") $page = "index";
  if ($page[0] == "/") $page="index";

  $lang = str_replace(".", "", $lang);
  $lang = str_replace("/", "", $lang);
  if ($lang == "") $lang = "en";

  if (!file_exists("p/$page")) $page="index";
  if (!file_exists("p/$page/$lang-body")) $lang="en";

  $title = read_var("p/$page/$lang-title");

/* replacements for usual <img> and <a href> etc, tags that fill in the
 * src, dest, thumb etc. for you */
  function img($src, $alt) {
      global $page;
      global $lang;
      list($width, $height, $type, $attr) = getimagesize("p/$page/d/$src");
      echo("<img src=\"p/$page/d/$src\" style='width:".$width."px; height:".$height."px; border:0;' alt='$alt'/>");
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
      echo("<a href='p.php?p=$pg&l=$lang'>$txt</a>");
  }

  function ast($pg) {
      global $page;
      global $lang;
      echo("<a href='p.php?p=$pg&l=$lang'>");
  }

  function acl($pg, $cl) {
      global $page;
      global $lang;
      echo("<a href='p.php?p=$pg&l=$lang' class='$cl'>");
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

  function nav_button($b, $c) {
      global $page;
      global $lang;

      if (file_exists("p/$b/$lang-label"))
	$l = read_var("p/$b/$lang-label");
      else
	$l = read_var("p/$b/en-label");
      if (file_exists("p/$b/page")) {
	  $h = read_var("p/$b/page");
	  $h = "p.php?p=$h&amp;l=$lang";
      }
      else if (file_exists("p/$b/link")) {
	  $h = read_var("p/$b/link");
      }
      return "<tr><td class='$c'><a class='nav' href='$h'>$l</a></td></tr>\n";
  }

  function nav_subs() {
      global $page;
      global $lang;
      $item = 0;
      if (file_exists("p/$page/subs")) {
	  $handle = fopen("p/$page/subs", "r");
	  if ($handle) {
	      echo("<ul class='navul'>\n");
	      while (!feof($handle)) {
		  $fl = fgets($handle, 4096);
		  $fl = str_replace("\n", "", $fl);
		  if ($fl != "") {
		      $h = "";
		      if (file_exists("p/$page/$fl/link")) {
			  $h = read_var("p/$page/$fl/link");
		      }
                          
		      if (file_exists("p/$page/$fl/$lang-label")) {
			  $l = read_var("p/$page/$fl/$lang-label");
			  if ($h == "") {
			    $h = "p.php?p=$page/$fl&amp;l=$lang";
                          }
			  if ($item > 0) echo("|\n");
			  echo("<li class='navul'><a class='navul' href='$h'>$l</a></li>\n");
			  $item++;
		      }
		      else if (file_exists("p/$page/$fl/en-label")) {
			  $l = read_var("p/$page/$fl/en-label");
			  if ($h == "") {
			    $h = "p.php?p=$page/$fl&amp;l=$lang";
                          }
			  if ($item > 0) echo("|\n");
			  echo("<li class='navul'><a class='navul' href='$h'>$l</a></li>\n");
			  $item++;
		      }
		  }
	      }
	      echo("</ul><hr>");
	  }
      }
  }
?>	
