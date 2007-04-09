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
      print("<img src='p/$page/d/$src' width=$width height=$height alt='$alt' border=0>");
  }

  function thm($src, $alt) {
      global $page;
      global $lang;
      list($width, $height, $type, $attr) = getimagesize("p/$page/d/.t.$src");
      print("<a href='p/$page/d/$src'><img src='p/$page/d/.t.$src' width=$width height=$height alt='$alt' class='thumb'></a>");
  }

  function a($pg, $txt) {
      global $page;
      global $lang;
      print("<a href='p.php?page=$pg?l=$lang'>$txt</a>");
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
		      print("<a href='p.php?p=$page&l=$fl'><img src='i/$fl.png' width=24 height=18></a>\n");
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
	  $h = "p.php?p=$h&l=$lang";
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
	      print("<ul class='navul'>\n");
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
			    $h = "p.php?p=$page/$fl&l=$lang";
                          }
			  if ($item > 0) print("|\n");
			  print("<li class='navul'><a class='navul' href='$h'>$l</a></li>\n");
			  $item++;
		      }
		      else if (file_exists("p/$page/$fl/en-label")) {
			  $l = read_var("p/$page/$fl/en-label");
			  if ($h == "") {
			    $h = "p.php?p=$page/$fl&l=$lang";
                          }
			  if ($item > 0) print("|\n");
			  print("<li class='navul'><a class='navul' href='$h'>$l</a></li>\n");
			  $item++;
		      }
		  }
	      }
	      print("</ul><hr>");
	  }
      }
  }
?>	
