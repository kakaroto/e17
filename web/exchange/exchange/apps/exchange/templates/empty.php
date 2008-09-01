<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>

<?php include_http_metas() ?>
<?php include_metas() ?>

<?php include_title() ?>
<?php if (has_slot('rss')): ?>
  <?php include_slot('rss') ?>
<?php endif; ?>
</head>
<body>
<table>
	<tr>
		<td class="content">
			<?php echo $sf_content ?>
		</td>
	</tr>
</table>
</body>
</html>
