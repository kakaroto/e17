<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" 
"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<!--
  -- index.php
  --
  -- Modified By: Nicholas "mekius" Hughart
  --       Added: Pagination Support
  --        Date: 2012-08-28
  -->
<html>
  <head>
    <title>Enlightenment Screenshots</title>
    <link href="index.css" rel="stylesheet" type="text/css"></link>
  </head>
  <body bgcolor=#ffffff alink=#88bbff link=#000000 vlink=#888888>
  <?php
    define('IMAGES_PER_PAGE', 24);
    define('MAX_PAGE_LINKS', 9);

    $page = 0;
    if (isset($_GET['page']))
      $page = (int)($_GET['page']);

    $files = glob("e-*");
    array_multisort(
                     array_map( 'filemtime', $files ),
                     SORT_NUMERIC,
                     SORT_DESC,
                     $files
                   );

    function extn($str) {
        $i = strrpos($str,".");
        if (!$i) { return ""; }
        $l = strlen($str) - $i;
        $ext = substr($str,$i+1,$l);
        return $ext;
    }

    function dothumb($f, $thumb, $new_w, $new_h) {
        $ext = extn($f);
        if (!strcmp("jpg", $ext))
          $src_img = imagecreatefromjpeg($f);
        if (!strcmp("png", $ext))
          $src_img = imagecreatefrompng($f);
        $old_x = imageSX($src_img);
        $old_y = imageSY($src_img);
        $ratio1 = $old_x / $new_w;
        $ratio2 = $old_y / $new_h;
        if ($ratio1 > $ratio2) {
          $thumb_w = $new_w;
          $thumb_h = $old_y / $ratio1;
        }
        else {
          $thumb_h = $new_h;
          $thumb_w = $old_x / $ratio2;
        }
        $dst_img = ImageCreateTrueColor($thumb_w, $thumb_h);
        imagecopyresampled($dst_img, $src_img, 0, 0, 0, 0,
                           $thumb_w, $thumb_h, $old_x, $old_y); 
        if (!strcmp("png", $ext))
          imagepng($dst_img, $thumb); 
        else
          imagejpeg($dst_img, $thumb);
        imagedestroy($dst_img);
        imagedestroy($src_img);
    }

    $pages = (int)(count($files) / IMAGES_PER_PAGE);
    if (count($files) % IMAGES_PER_PAGE)
      $pages++;
  ?>

    <div id="PagesWrapper">
      <div id="Pages">
      <?php
        echo '<span class="PreviousArrows">';
        if ($page != 0)
          echo "<span><a href='?page=0'>&laquo;</a></span>";
        else
          echo "<span>&laquo;</span>";

        if ($page > 0)
          echo "<span><a href='?page=".($page-1)."' accesskey='p'>&lt;</a></span>";
        else
          echo "<span>&lt;</span>";
        echo '</span>';

        $i = 0;
        if (
            ($pages > MAX_PAGE_LINKS) && 
            ($page > (MAX_PAGE_LINKS/2))
           )
        {
          if ($page > ($pages - (MAX_PAGE_LINKS/2)))
            $i = $pages - MAX_PAGE_LINKS;
          else
            $i = $page - (int)(MAX_PAGE_LINKS/2);
        }

        for ($j = 0; ($i < $pages) && ($j < MAX_PAGE_LINKS); ++$i, ++$j)
        {
          if ($i == $page)
            echo "<a href=\"?page=$i\" class='highlight'>".sprintf("%02u", $i+1)."</a>\n";
          else
            echo "<a href=\"?page=$i\">".sprintf("%02u", $i+1)."</a>\n";
        }

        echo '<span class="NextArrows">';
        if ($page < ($pages-1))
          echo "<span><a href='?page=".($page+1)."' accesskey='n'>&gt;</a></span>";
        else
          echo "<span>&gt;</span>";

        if ($page != ($pages-1))
          echo "<span><a href='?page=".($pages-1)."' >&raquo;</a></span>";
        else
          echo "<span>&raquo;</span>";
        echo '</span>';
      ?>
      </div>
    </div>

    <div id="Images">
    <?php
      $skip = $page * IMAGES_PER_PAGE;
      foreach ($files as &$f) {
          if ($skip-- > 0)
            continue;

          if ($skip < -IMAGES_PER_PAGE)
            break;

          $thumb = "th-" . $f;
          if (!file_exists($thumb)) {
              dothumb($f, $thumb, 320, 240);
          }
          print "<a href=" . $f . "><img src=" . $thumb . " border=1 hspace=10 vspace=10></a>\n";
      }
    ?>
    </div>
  </body>
</html>