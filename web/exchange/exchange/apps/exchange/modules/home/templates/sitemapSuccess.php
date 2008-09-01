<?php echo '<?xml version="1.0" encoding="utf-8" ?>'?>
<urlset
  xmlns="http://www.google.com/schemas/sitemap/0.84"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:schemaLocation="http://www.google.com/schemas/sitemap/0.84
                      http://www.google.com/schemas/sitemap/0.84/sitemap.xsd">
<?php foreach ($urls as $url): ?>
	<url>
		<loc><?php echo $url['url'] ?></loc>
		<priority><?php echo $url['priority'] ?></priority>
		<changefreq><?php echo $url['changefreq'] ?></changefreq>
		<lastmod><?php echo date('c', strtotime($url['lastmod'])) ?></lastmod>
	</url>
<?php endforeach; ?>
</urlset>