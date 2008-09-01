<table class="table">
	<tbody>
		<?php if ($themes && count($themes)): ?>
			<?php $count = 0; ?>
			<?php foreach ($themes as $theme): ?>
				<?php $count++ ?>
				<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
					<td class="smallthumb">
						<a href="/theme/show/<?php echo $theme->getId() ?>" class="thumb">
							<img src="<?php echo $theme->getUrlPath() ?>smallthumb.png" alt="<?php echo $theme->getName() ?> Thumbnail" ?>
						</a>
						<div>
						<a href="<?php echo $theme->getUrlPath() ?>bigthumb.png" class="bigthumb">
							<img src="/images/magnifier.png" alt="<?php echo $theme->getName() ?> Thumbnail" ?>
						</a>
						</div>
					</td>
					<td class="main">
						<a href="/theme/show/<?php echo $theme->getId() ?>"><?php echo $theme->getName() ?></a>
						<?php if (!$theme->getApproved()): ?>[<span class="red">Not Approved</span>]<?php endif; ?>
						<?php include_component('rating', 'rating', array('rating' => $theme->getRating(), 'theme_id' => $theme->getId(), 'odd' => !($count%2))) ?>
						<div class="description">
						<?php  $description = $theme->getDescription();
						  if (strlen($description) > 200)
						  	$description = substr($description, 0, 200).'...';
						  $wpwiki = new WikiText();
						  echo $wpwiki->transform("\n".$description."\n");
						?>
						</div>
						<?php if (($sf_user->getId() == $theme->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
							<?php echo link_to('Edit', '/theme/update?id='.$theme->getId()) ?> |
							<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/theme/delete/'.$theme->getId().'\')')) ?>
						<?php endif; ?>
					</td>
				</tr>
			<?php endforeach; ?>
		<?php else: ?>
			<tr>
				<td>
					There are no themes here.
				</td>
			</tr>
		<?php endif; ?>
	</tbody>
</table>

<?php if (count($themes)): ?>
<div class="table_sort">
<span class="right">
	<?php if ($showcase): ?><?php echo link_to('Show All Themes', $allThemes) ?> | <?php endif; ?>
	<?php if($prevPage): ?><?php echo link_to('&laquo; Previous', $prevPage) ?> | <?php endif; ?>
	Showing <?php echo $range ?>
	<?php if($nextPage): ?> | <?php echo link_to('Next &raquo;', $nextPage) ?><?php endif; ?>
</span>
<form method="post" action="/theme/sort">
	Sort: <?php echo $form['sort'] ?><?php echo $form['order'] ?> &nbsp;&nbsp;Show: <?php echo $form['limit'] ?>
</form>
</div>
<?php endif; ?>