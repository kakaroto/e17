<table class="table">
	<tbody>
		<?php if ($comments && count($comments)): ?>
			<?php $count = 0; ?>
			<?php foreach ($comments as $comment): ?>
				<?php $count++ ?>
				<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
					<td>
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
					</td>
				</tr>
			<?php endforeach; ?>
		<?php else: ?>
			<tr>
				<td>
					There are no comments here.
				</td>
			</tr>
		<?php endif; ?>
		<?php if ($sf_user->hasCredential('user')): ?>
			<?php if(isset($application_id)||isset($module_id)||isset($theme_id)): ?>
			<tr>
				<td class="comment_form">
					<form method="post" action="/comment/update" >
					<div><?php echo $form['comment'] ?></div>
					<div><?php echo $form['application_id'] ?><?php echo $form['module_id'] ?><?php echo $form['theme_id'] ?>
						    	<input type="submit" value="Add Comment" />
					</div>
					</form>
				</td>
			</tr>
			<?php endif; ?>
		<?php else: ?>
			<tr>
				<td><?php echo link_to('Login', '/user/login') ?> to add a comment.</td>
			</tr>
		<?php endif; ?>
	</tbody>
</table>
<?php if (count($comments)): ?>
<div class="table_sort">
<span class="right">
	<?php if($prevPage): ?><?php echo link_to('&laquo; Previous', $prevPage) ?> | <?php endif; ?>
	Showing <?php echo $range ?>
	<?php if($nextPage): ?> | <?php echo link_to('Next &raquo;', $nextPage) ?><?php endif; ?>
</span>
<form method="post" action="/comment/sort">
	Show: <?php echo $sortForm['limit'] ?> 
</form>
</div>
<?php endif; ?>