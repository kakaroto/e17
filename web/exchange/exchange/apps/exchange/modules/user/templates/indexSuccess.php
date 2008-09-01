<?php slot('title') ?>Users<?php end_slot() ?>
<h1>Users</h1>

<?php include_component('user', 'table') ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="Users" href="<?php echo Tools::get('url').'/rss/user' ?>" />
<?php end_slot() ?>