<?php slot('title') ?>Comment<?php end_slot() ?>
<div class="read">
<div class="description">
<?php  $wpwiki = new WikiText();
  echo $wpwiki->transform("\n".$comment->getComment()."\n");
?>
</div>
<div class="author">By <a href="/user/show/<?php echo $comment->getUserId() ?>"><?php echo $comment->getUser()->getName() ?></a>
on <?php echo $comment->getUpdatedAt() ?> for <a href="<?php echo $comment->getObject()->getShowPath() ?>"><?php echo $comment->getObject()->getName() ?></a>
<?php if (($sf_user->getId() == $comment->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
	[ <?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/comment/delete/'.$comment->getId().'\')')) ?> ]
<?php endif; ?>
</div>
</div>