<?php
header("Content-type: application/rss+xml");
echo '<?xml version="1.0"?>'
?>
<?php include 'site/site.php'; ?>
<rss version="2.0">
  <channel>
    <title><?php echo "$title"; ?></title>
    <link>http://enlightenment.org/p.php?p=<?php echo "$page"; ?>&amp;l=<?php echo "$lang"; ?></link>
    <language><?php echo "$lang"; ?></language>
    <pubDate><?php echo strftime ("%a, %d %b %Y %H:%M:%S %z") ;?></pubDate>
    <docs>http://blogs.law.harvard.edu/tech/rss</docs>
    <generator>Enlightenment's RSS generator</generator>
    <managingEditor>barbieri@profusion.mobi</managingEditor>
    <webMaster>barbieri@profusion.mobi</webMaster>

    <?php
      if (file_exists("p/$page/$lang-rss"))
        include "p/$page/$lang-rss";
    ?>
  </channel>
</rss>
