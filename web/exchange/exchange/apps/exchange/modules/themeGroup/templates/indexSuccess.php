<?php slot('title') ?>Theme Groups<?php if ($theme): ?> for <?php echo $theme->getName() ?><?php endif; ?><?php end_slot() ?>
<h1>Theme Groups<?php if ($theme): ?> for <?php echo $theme->getName() ?><?php endif; ?></h1>
<?php include_component('themeGroup', 'table') ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="ThemeGroups" href="<?php echo Tools::get('url').'/rss/theme_group' ?>" />
<?php end_slot() ?>