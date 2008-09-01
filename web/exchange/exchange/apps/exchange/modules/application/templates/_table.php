<table class="table">
	<tbody>
		<?php if ($applications && count($applications)): ?>
			<?php $count = 0; ?>
			<?php foreach ($applications as $application): ?>
				<?php $count++ ?>
				<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
					<td class="smallthumb">
						<a href="/application/show/<?php echo $application->getId() ?>" class="thumb">
							<img src="<?php echo $application->getUrlPath() ?>smallthumb.png" alt="<?php echo $application->getName() ?> Thumbnail" ?>
						</a>
						<div>
						<a href="<?php echo $application->getUrlPath() ?>bigthumb.png" class="bigthumb">
							<img src="/images/magnifier.png" alt="<?php echo $application->getName() ?> Thumbnail" ?>
						</a>
						</div>
					</td>
					<td class="main">
						<a href="/application/show/<?php echo $application->getId() ?>"><?php echo $application->getName() ?></a>
						<?php if (!$application->getApproved()): ?>[<span class="red">Not Approved</span>]<?php endif; ?>
						<?php include_component('rating', 'rating', array('rating' => $application->getRating(), 'application_id' => $application->getId(), 'odd' => !($count%2))) ?>
						<div class="description">
						<?php  $description = $application->getDescription();
						  if (strlen($description) > 200)
						  	$description = substr($description, 0, 200).'...';
						  $wpwiki = new WikiText();
						  echo $wpwiki->transform("\n".$description."\n");
						?>
						</div>
						<?php if (($sf_user->getId() == $application->getUserId()) || ($sf_user->hasCredential('admin'))): ?>
							<?php echo link_to('Edit', '/application/update?id='.$application->getId()) ?> |
							<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/application/delete/'.$application->getId().'\')')) ?>
						<?php endif; ?>
					</td>
				</tr>
			<?php endforeach; ?>
		<?php else: ?>
			<tr>
				<td>
					There are no applications here.
				</td>
			</tr>
		<?php endif; ?>
	</tbody>
</table>
<?php if (count($applications)): ?>
<div class="table_sort">
<span class="right">
	<?php if ($showcase): ?><?php echo link_to('Show All Applications', $allApplications) ?> | <?php endif; ?>
	<?php if($prevPage): ?><?php echo link_to('&laquo; Previous', $prevPage) ?> | <?php endif; ?>
	Showing <?php echo $range ?>
	<?php if($nextPage): ?> | <?php echo link_to('Next &raquo;', $nextPage) ?><?php endif; ?>
</span>
<form method="post" action="/application/sort">
	Sort: <?php echo $form['sort'] ?><?php echo $form['order'] ?> &nbsp;&nbsp;Show: <?php echo $form['limit'] ?>
</form>
</div>
<?php endif; ?>