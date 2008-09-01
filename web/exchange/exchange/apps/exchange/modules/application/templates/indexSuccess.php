<?php slot('title') ?>Applications
	<?php if($user): ?>by <?php echo $user->getName() ?><?php endif; ?><?php end_slot() ?>
<h1>Applications
	<?php if($user): ?>by <?php echo $user->getName() ?><?php endif; ?>
</h1>
<?php if ($sf_user->hasCredential('user')): ?>
	<?php echo link_to('Add new Application', '/application/create') ?>
<?php endif; ?>

<?php include_component('application', 'table') ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="Applications" href="<?php echo Tools::get('url').'/rss/application' ?>" />
<?php end_slot() ?>
