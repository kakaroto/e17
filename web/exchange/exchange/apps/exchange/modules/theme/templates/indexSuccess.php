<?php slot('title') ?>Themes
	<?php if($user): ?>by <?php echo $user->getName() ?><?php endif; ?>
	<?php if($application): ?>for <?php echo $application->getName() ?><?php endif; ?>
	<?php if($madule): ?>for <?php echo $madule->getName() ?><?php endif; ?>
	<?php if($theme_group): ?>for <?php echo $theme_group->getDisplayName() ?><?php endif; ?><?php end_slot() ?>
<h1>Themes
	<?php if($user): ?>by <?php echo $user->getName() ?><?php endif; ?>
	<?php if($application): ?>for <?php echo $application->getName() ?><?php endif; ?>
	<?php if($madule): ?>for <?php echo $madule->getName() ?><?php endif; ?>
	<?php if($theme_group): ?>for <?php echo $theme_group->getDisplayName() ?><?php endif; ?>
</h1>
<?php if ($sf_user->hasCredential('user')): ?>
	<?php echo link_to('Add new Theme', '/theme/create') ?>
<?php endif; ?>
<?php if ($sf_user->hasCredential('admin')): ?>
	| <?php echo link_to('Theme Groups', '/themeGroup/index') ?>
<?php endif; ?>

<?php include_component('theme', 'table') ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="Themes" href="<?php echo Tools::get('url').'/rss/theme' ?>" />
<?php end_slot() ?>