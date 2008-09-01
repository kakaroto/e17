<?php slot('title') ?>Theme: <?php echo $theme->getName() ?><?php end_slot() ?>
<div class="read">
<h1><?php echo $theme->getName() ?></h1>
<div class="download">
<?php echo link_to('Download', '/theme/download?id='.$theme->getId()) ?>
</div>
<h3>
Uploaded by <a href="/user/show/<?php echo $theme->getUserId() ?>"><?php echo $theme->getUser()->getName() ?></a> on <?php echo $theme->getUpdatedAt() ?>
 <?php if (!$theme->getApproved()): ?>[<span class="red">Not Approved</span>]<?php endif; ?>
</h3>
<?php if (($sf_user->getId() == $theme->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
	<?php echo link_to('Edit', '/theme/update?id='.$theme->getId()) ?> |
	<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/theme/delete/'.$theme->getId().'\')')) ?>
<?php endif; ?>
<?php if ($sf_user->hasCredential('admin')): ?>
<?php if ($theme->getApproved()): ?><?php $approve_text = 'Disapprove' ?><?php else: ?><?php $approve_text = 'Approve' ?><?php endif; ?>
<?php if ($theme->getShowcase()): ?><?php $showcase_text = 'Remove from showcase' ?><?php else: ?><?php $showcase_text = 'Add to showcase' ?><?php endif; ?>
 | <?php echo link_to($approve_text, '/theme/approve?id='.$theme->getId()) ?> | <?php echo link_to($showcase_text, '/theme/showcase?id='.$theme->getId()) ?>
<?php endif; ?>
<?php include_component('rating', 'rating', array('rating' => $theme->getRating(), 'theme_id' => $theme->getId(), 'odd' => false)) ?>
Downloads: <?php echo intval($theme->getDownloads()) ?>
<table class="metadata">
<tbody>
<?php if ($theme->getAuthor()): ?>
<th>Author</th><td><?php echo $theme->getAuthor() ?></td></tr>
<?php endif; ?>
<?php if ($theme->getLicense()): ?>
<tr><th>License</th><td><?php echo $theme->getLicense() ?></td></tr>
<?php endif; ?>
<?php if ($theme->getVersion()): ?>
<tr><th>Version</th><td><?php echo $theme->getVersion() ?></td></tr>
<?php endif; ?>
</tbody>
</table>
<div class="description">
<?php  $wpwiki = new WikiText();
  echo $wpwiki->transform("\n".$theme->getDescription()."\n");
?>
</div>
<div class="screenshot">
<a href="<?php echo $theme->getUrlPath() ?>screenshot.png">
<img src="<?php echo $theme->getUrlPath() ?>bigthumb.png" alt="<?php echo $theme->getName() ?> Thumbnail" ?>
</a>
</div>
<br/>
<h2>Provides</h2>
<?php include_component('themeGroup', 'table', array('theme_id' => $theme->getId(), 'compact' => true)) ?>
<br/>
<br/>
<h2>Comments</h2>
<?php include_component('comment', 'table', array('theme_id' => $theme->getId())) ?>
</div>
<?php slot('rss') ?>
<link rel="alternate" type="application/rss+xml" title="<?php echo $theme->getName() ?>'s Comments" href="<?php echo Tools::get('url').'/rss/comment/theme_id/'.$theme->getId() ?>" />
<?php end_slot() ?>
