<?php slot('title') ?>Modules
	<?php if($user): ?>by <?php echo $user->getName() ?><?php endif; ?>
	<?php if($application): ?>for <?php echo $application->getName() ?><?php endif; ?><?php end_slot() ?>
<h1>Modules
	<?php if($user): ?>by <?php echo $user->getName() ?><?php endif; ?>
	<?php if($application): ?>for <?php echo $application->getName() ?><?php endif; ?>
</h1>
<?php if ($sf_user->hasCredential('user')): ?>
	<?php echo link_to('Add new Module', '/module/create') ?>
<?php endif; ?>

<?php include_component('module', 'table') ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="Modules" href="<?php echo Tools::get('url').'/rss/module' ?>" />
<?php end_slot() ?>
