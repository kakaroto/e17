<table class="table">
	<tbody>
		<?php if ($madules && count($madules)): ?>
			<?php $count = 0; ?>
			<?php foreach ($madules as $madule): ?>
				<?php $count++ ?>
				<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
					<td class="smallthumb">
						<a href="/module/show/<?php echo $madule->getId() ?>" class="thumb">
							<img src="<?php echo $madule->getUrlPath() ?>smallthumb.png" alt="<?php echo $madule->getName() ?> Thumbnail" ?>
						</a>
						<div>
						<a href="<?php echo $madule->getUrlPath() ?>bigthumb.png" class="bigthumb">
							<img src="/images/magnifier.png" alt="<?php echo $madule->getName() ?> Thumbnail" ?>
						</a>
						</div>
					</td>
					<td class="main">
						<a href="/module/show/<?php echo $madule->getId() ?>"><?php echo $madule->getName() ?></a>
						<?php if (!$madule->getApproved()): ?>[<span class="red">Not Approved</span>]<?php endif; ?>
						<?php include_component('rating', 'rating', array('rating' => $madule->getRating(), 'module_id' => $madule->getId(), 'odd' => !($count%2))) ?>
						<div class="description">
						<?php  $description = $madule->getDescription();
						  if (strlen($description) > 200)
						  	$description = substr($description, 0, 200).'...';
						  $wpwiki = new WikiText();
						  echo $wpwiki->transform("\n".$description."\n");
						?>
						</div>
						<?php if (($sf_user->getId() == $madule->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
							<?php echo link_to('Edit', '/module/update?id='.$madule->getId()) ?> |
							<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/module/delete/'.$madule->getId().'\')')) ?>
						<?php endif; ?>
					</td>
				</tr>
			<?php endforeach; ?>
		<?php else: ?>
			<tr>
				<td>
					There are no modules here.
				</td>
			</tr>
		<?php endif; ?>
	</tbody>
</table>

<?php if (count($madules)): ?>
<div class="table_sort">
<span class="right">
	<?php if ($showcase): ?><?php echo link_to('Show All Modules', $allModules) ?> | <?php endif; ?>
	<?php if($prevPage): ?><?php echo link_to('&laquo; Previous', $prevPage) ?> | <?php endif; ?>
	Showing <?php echo $range ?>
	<?php if($nextPage): ?> | <?php echo link_to('Next &raquo;', $nextPage) ?><?php endif; ?>
</span>
<form method="post" action="/module/sort">
	Sort: <?php echo $form['sort'] ?><?php echo $form['order'] ?> &nbsp;&nbsp;Show: <?php echo $form['limit'] ?> 
</form>
</div>
<?php endif; ?>