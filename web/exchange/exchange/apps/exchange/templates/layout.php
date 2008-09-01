<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>

<?php include_http_metas() ?>
<?php include_metas() ?>

<?php if (has_slot('title')): ?>
<title><?php include_slot('title') ?> - Exchange</title>
<?php else: ?>
<?php include_title() ?>
<?php endif; ?>
<?php if (has_slot('rss')): ?>
<?php include_slot('rss') ?>
<?php endif; ?>
</head>
<body>
<div class="header">
	<a href="/"><img src="/images/logo.png" alt="<?php echo Tools::get('name') ?>" /></a>
	<div class="right">
		<?php $searchForm = new SearchForm() ?>
		<form method="post" action="/home/search">
			<?php if ($sf_user->hasCredential('user')): ?>
				Hello <?php echo link_to($sf_user->getUser()->getName(), '/user/read?id='.$sf_user->getId()) ?>
			<?php else: ?>
				<?php echo link_to('Login', '/user/login') ?>
			<?php endif; ?>
			| Search: <?php echo $searchForm['term'] ?>
		</form>
	</div> 
</div>
<table>
	<tr>
		<td class="nav">
			<ul>
				<?php include_component('home', 'navigation') ?>
			</ul>
		</td>
		<td class="content">
			<?php echo $sf_content ?>
		</td>
	</tr>
</table>
<div class="footer">
<?php echo Tools::get('copyright') ?> | <a href="/home/about">About</a>
</div>


<script type="text/javascript">
/* <![CDATA[ */
window.addEvent('load', function() {

	/**
	 * Default options for that one, except the origin. Getting the coordinates for the
	 * <img> element inside allows a correct positioning for the remooz clone-box.
	 * (Mainly becaue the <a> element is not exactly wrapped around the image)
	 */

	ReMooz.assign('.smallthumb a.bigthumb', {
		origin: 'img'
	});

});
/* ]]> */
</script>
<?php if($sf_user->hasCredential('user')): ?>
<script type="text/javascript">
window.addEvent('domready', function(e) {
	var rating = new RabidRatings({url:'/rating/rate'});
});
</script>
<?php else: ?>
<script type="text/javascript">
window.addEvent('domready', function(e) {
		$$('.rabidRating').each(function(el) {
			el.wrapper = el.getElement('.wrapper');
			el.wrapper.addEvent('click', function(e) {
				window.location = "/user/login";
			}.bind(this));
		});
});
</script>
<?php endif; ?>
</body>
</html>
