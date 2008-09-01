<?php use_helper('Form') ?>
<?php if (!$compact): ?>
<table class="table">
	<tbody>
		<?php if ($themeGroups && count($themeGroups)): ?>
			<?php $count = 0; ?>
			<?php foreach ($themeGroups as $themeGroup): ?>
				<?php $count++ ?>
				<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
					<td>
						<?php echo $themeGroup->getTitle() ?>
						<?php if ($sf_user->hasCredential('admin')): ?>
							[<?php if ($themeGroup->getKnown()): ?><span class="green">Known</span><?php else: ?><span class="red">Unknown</span><?php endif; ?>]
						<?php endif; ?>
						<?php if ($themeGroup->getTitle()): ?><br/><?php endif; ?>
						<?php echo link_to($themeGroup->getName(), '/themeGroup/read?id='.$themeGroup->getId()) ?>
					</td>
					<td>
						<?php if ($sf_user->hasCredential('admin')): ?>
							<?php echo link_to('Edit', '/themeGroup/update?id='.$themeGroup->getId()) ?>
							<?php if ($application_id): ?>
								| <?php echo link_to('Remove Association', '/applicationThemeGroup/remove?application_id='.$application_id.'&theme_group_id='.$themeGroup->getId()) ?>
							<?php endif; ?>
							<?php if ($module_id): ?>
								| <?php echo link_to('Remove Association', '/moduleThemeGroup/remove?module_id='.$module_id.'&theme_group_id='.$themeGroup->getId()) ?>
							<?php endif; ?>
						<?php endif; ?>
					</td>
				</tr>
			<?php endforeach; ?>
		<?php else: ?>
			<tr>
				<td>
					There are no theme groups here.
				</td>
			</tr>
		<?php endif; ?>
	</tbody>
</table>
<?php else: ?>
		<?php if ($themeGroups && count($themeGroups)): ?>
			<?php $count = 0; ?>
			<?php foreach ($themeGroups as $themeGroup): ?>
				<?php $count++ ?>
				<?php echo link_to($themeGroup->getTitle(), '/themeGroup/read?id='.$themeGroup->getId()) ?>
				<?php if ($count < count($themeGroups)): ?> | <?php endif; ?>
			<?php endforeach; ?>
			| 
		<?php else: ?>
			There are no known groups.
		<?php endif; ?>
		<?php echo link_to('View Complete List &raquo;', $morePage) ?>
<?php endif; ?>