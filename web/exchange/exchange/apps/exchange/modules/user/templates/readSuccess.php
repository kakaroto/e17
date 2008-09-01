<?php slot('title') ?>User: <?php echo $user->getName() ?><?php end_slot() ?>
<div class="read">
<h1><?php echo $user->getName() ?></h1>
<h3><?php if ($sf_user->hasCredential('admin')): ?>Email Address: <?php echo $user->getEmail() ?><?php endif; ?></h3>
<?php if (!$user->getActive()): ?>[<span class="red">Not Active</span>]<?php endif; ?>
<?php if (($sf_user->getId() == $user->getId()) || ($sf_user->hasCredential('admin'))): ?>
	<?php echo link_to('Edit', '/user/update?id='.$user->getId()) ?>
<?php endif; ?>
<h2>Themes by this user</h2>
<?php include_component('theme', 'table', array('user_id' => $user->getId())) ?>
<h2>Applications by this user</h2>
<?php include_component('application', 'table', array('user_id' => $user->getId())) ?>
<h2>Modules by this user</h2>
<?php include_component('module', 'table', array('user_id' => $user->getId())) ?>
<h2>Comments made by this user</h2>
<?php include_component('comment', 'table', array('user_id' => $user->getId())) ?>
</div>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="<?php echo $user->getName() ?>'s Themes" href="<?php echo Tools::get('url').'/rss/theme/user_id/'.$user->getId() ?>" />
<link rel="alternate" type="application/rss+xml" title="<?php echo $user->getName() ?>'s Modules" href="<?php echo Tools::get('url').'/rss/module/user_id/'.$user->getId() ?>" />
<link rel="alternate" type="application/rss+xml" title="<?php echo $user->getName() ?>'s Comments" href="<?php echo Tools::get('url').'/rss/comment/user_id/'.$user->getId() ?>" />
<?php end_slot() ?>