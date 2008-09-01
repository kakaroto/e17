<?php use_helper('Form') ?>
<table class="table">
	<tbody>
		<?php if ($users && count($users)): ?>
			<?php $count = 0; ?>
			<?php foreach ($users as $user): ?>
				<?php $count++ ?>
				<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
					<td>
						<a href="/user/show/<?php echo $user->getId() ?>">
							<?php echo $user->getName() ?>
						</a>
					</td>
					<td>
						<?php echo $user->getRoleName() ?>
					</td>
					<td>
						<?php if ($sf_user->hasCredential('admin')): ?>
							<?php echo link_to('Edit', '/user/update?id='.$user->getId()) ?> |
							<?php echo link_to('Delete', '#', array('onClick' => 'confirmFirst(\'/user/delete/'.$user->getId().'\')')) ?>
						<?php endif; ?>
					</td>
				</tr>
			<?php endforeach; ?>
		<?php else: ?>
			<tr>
				<td>
					There are no users here.
				</td>
			</tr>
		<?php endif; ?>
	</tbody>
</table>

<?php if (count($users)): ?>
<div class="table_sort">
<span class="right">
	<?php if($prevPage): ?><?php echo link_to('&laquo; Previous', $prevPage) ?> | <?php endif; ?>
	Showing <?php echo $range ?>
	<?php if($nextPage): ?> | <?php echo link_to('Next &raquo;', $nextPage) ?><?php endif; ?>
</span>
<form method="post" action="/user/sort">
	Show: <?php echo $form['limit'] ?> 
</form>
</div>
<?php endif; ?>