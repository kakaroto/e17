<?php slot('title') ?>Module: <?php echo $madule->getName() ?><?php end_slot() ?>
<div class="read">
<h1><?php echo $madule->getName() ?></h1>
<div class="download">
<a href="<?php echo $madule->getSourceDisplayUrl() ?>">Get It</a>
</div>
<h3>
Updated by <a href="/user/show/<?php echo $madule->getUserId() ?>"><?php echo $madule->getUser()->getName() ?></a> on <?php echo $madule->getUpdatedAt() ?>
 <?php if (!$madule->getApproved()): ?>[<span class="red">Not Approved</span>]<?php endif; ?>
</h3>
<?php if (($sf_user->getId() == $madule->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
	<?php echo link_to('Edit', '/module/update?id='.$madule->getId()) ?> |
	<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/module/delete/'.$madule->getId().'\')')) ?>
<?php endif; ?>
<?php if ($sf_user->hasCredential('admin')): ?>
<?php if ($madule->getApproved()): ?><?php $approve_text = 'Disapprove' ?><?php else: ?><?php $approve_text = 'Approve' ?><?php endif; ?>
<?php if ($madule->getShowcase()): ?><?php $showcase_text = 'Remove from showcase' ?><?php else: ?><?php $showcase_text = 'Add to showcase' ?><?php endif; ?>
 | <?php echo link_to('Associate with Theme Group', '/moduleThemeGroup/update?module_id='.$madule->getId()) ?> | <?php echo link_to($approve_text, '/module/approve?id='.$madule->getId()) ?> | <?php echo link_to($showcase_text, '/module/showcase?id='.$madule->getId()) ?>
<?php endif; ?>
<?php include_component('rating', 'rating', array('rating' => $madule->getRating(), 'module_id' => $madule->getId(), 'odd' => false)) ?>
<div class="description">
<?php  $wpwiki = new WikiText();
  echo $wpwiki->transform("\n".$madule->getDescription()."\n");
?>
</div>
<div class="screenshot">
<a href="<?php echo $madule->getUrlPath() ?>screenshot.png">
<img src="<?php echo $madule->getUrlPath() ?>bigthumb.png" alt="<?php echo $madule->getName() ?> Thumbnail" ?>
</a>
</div>
<br/>
<?php if ($hasThemes): ?>
<h2>Themes for <?php echo $madule->getName() ?></h2>
<?php include_component('theme', 'table', array('module_id' => $madule->getId())) ?>
<?php endif; ?>
<h2>Comments</h2>
<?php include_component('comment', 'table', array('module_id' => $madule->getId())) ?>
</div>
<?php if ($sf_user->hasCredential('admin')): ?>
<h2>This Application is associated with</h2>
<?php include_component('themeGroup', 'table', array('module_id' => $madule->getId())) ?>
<?php endif; ?>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="<?php echo $madule->getName() ?>'s Comments" href="<?php echo Tools::get('url').'/rss/comment/module_id/'.$madule->getId() ?>" />
<?php end_slot() ?>