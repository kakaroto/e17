<?php slot('title') ?>Application: <?php echo $application->getName() ?><?php end_slot() ?>
<div class="read">
<h1><?php echo $application->getName() ?></h1>
<div class="download">
<a href="<?php echo $application->getSourceDisplayUrl() ?>">Get It</a>
</div>
<h3>
Updated by <a href="/user/show/<?php echo $application->getUserId() ?>"><?php echo $application->getUser()->getName() ?></a> on <?php echo $application->getUpdatedAt() ?>
 <?php if (!$application->getApproved()): ?>[<span class="red">Not Approved</span>]<?php endif; ?>
</h3>
<?php if (($sf_user->getId() == $application->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
	<?php echo link_to('Edit', '/application/update?id='.$application->getId()) ?> |
	<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/application/delete/'.$application->getId().'\')')) ?> |
<?php endif; ?>
<?php if ($sf_user->hasCredential('user')): ?>
	<?php echo link_to('Add new Module', '/module/update?application_id='.$application->getId()) ?>
<?php endif; ?>
<?php if ($sf_user->hasCredential('admin')): ?>
<?php if ($application->getApproved()): ?><?php $approve_text = 'Disapprove' ?><?php else: ?><?php $approve_text = 'Approve' ?><?php endif; ?>
<?php if ($application->getShowcase()): ?><?php $showcase_text = 'Remove from showcase' ?><?php else: ?><?php $showcase_text = 'Add to showcase' ?><?php endif; ?>
 | <?php echo link_to('Associate with Theme Group', '/applicationThemeGroup/update?application_id='.$application->getId()) ?> | <?php echo link_to($approve_text, '/application/approve?id='.$application->getId()) ?> | <?php echo link_to($showcase_text, '/application/showcase?id='.$application->getId()) ?>
<?php endif; ?>
<?php include_component('rating', 'rating', array('rating' => $application->getRating(), 'application_id' => $application->getId(), 'odd' => false)) ?>
<div class="description">
<?php  $wpwiki = new WikiText();
  echo $wpwiki->transform("\n".$application->getDescription()."\n");
?>
</div>
<div class="screenshot">
<a href="<?php echo $application->getUrlPath() ?>screenshot.png">
<img src="<?php echo $application->getUrlPath() ?>bigthumb.png" alt="<?php echo $application->getName() ?> Thumbnail" ?>
</a>
</div>
<br/>
<?php if ($hasModules): ?>
<h2>Modules for <?php echo $application->getName() ?></h2>
<?php include_component('module', 'table', array('application_id' => $application->getId())) ?>
<?php endif; ?>
<?php if ($hasThemes): ?>
<h2>Themes for <?php echo $application->getName() ?></h2>
<?php include_component('theme', 'table', array('application_id' => $application->getId())) ?>
<?php endif; ?>
<h2>Comments</h2>
<?php include_component('comment', 'table', array('application_id' => $application->getId())) ?>
</div>
<?php if ($sf_user->hasCredential('admin')): ?>
<h2>This Application is associated with</h2>
<?php include_component('themeGroup', 'table', array('application_id' => $application->getId())) ?>
<?php endif; ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="<?php echo $application->getName() ?>'s Comments" href="<?php echo Tools::get('url').'/rss/comment/application_id/'.$application->getId() ?>" />
<?php end_slot() ?>