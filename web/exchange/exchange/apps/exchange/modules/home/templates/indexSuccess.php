<?php slot('title') ?>Home<?php end_slot() ?>
<h1>Themes - Showcase</h1>
<?php include_component('theme', 'table', array('showcase' => true)) ?>
<br/>
<h1>Applications - Showcase</h1>
<?php include_component('application', 'table', array('showcase' => true)) ?>
<br/>
<h1>E Modules - Showcase</h1>
<?php include_component('module', 'table', array('application_id' => 4, 'showcase' => true)) ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="Themes" href="<?php echo Tools::get('url').'/rss/theme' ?>" />
<link rel="alternate" type="application/rss+xml" title="Applications" href="<?php echo Tools::get('url').'/rss/application' ?>" />
<?php end_slot() ?>