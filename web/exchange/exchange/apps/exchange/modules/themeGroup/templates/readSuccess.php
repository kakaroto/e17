<?php slot('title') ?>Themes for <?php echo $themeGroup->getDisplayName() ?><?php end_slot() ?>
<div class="read">
<h1>Themes for <?php echo $themeGroup->getDisplayName() ?></h1>
<h3>
<?php if ($sf_user->hasCredential('admin')): ?>
	<?php echo $themeGroup->getName() ?> [<?php if ($themeGroup->getKnown()): ?><span class="green">Known</span><?php else: ?><span class="red">Unknown</span><?php endif; ?>]
<?php endif; ?>
</h3>
<?php if ($sf_user->hasCredential('admin')): ?>
	<?php echo link_to('Edit Theme Group', '/themeGroup/update?id='.$themeGroup->getId()) ?> |
<?php endif; ?>
<?php if ($sf_user->hasCredential('user')): ?>
	<?php echo link_to('Add new Theme', '/theme/create') ?>
<?php endif; ?>
<?php include_component('theme', 'table', array('theme_group_id' => $themeGroup->getId())) ?>
</div>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="Themes - <?php echo $themeGroup->getTitle() ?>" href="<?php echo Tools::get('url').'/rss/theme_group_id/'.$themeGroup->getId() ?>" />
<?php end_slot() ?>