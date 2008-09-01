<li <?php if ($module=='home'): ?>class="active"<?php endif; ?> >
	<?php echo link_to('Home', '/home/index') ?>
</li>
<li <?php if ($module=='theme' || ($module=='themeGroup')): ?>class="active"<?php endif; ?> >
	<?php echo link_to('Themes', '/theme/index', array('class' => 'category')) ?>
</li>
<?php if ($module=='theme' || $module=='themeGroup'): ?>
<?php $heading = '' ?>
<?php $currentHeading = '' ?>
<?php if ($currentThemeGroup && $pos = stripos($currentThemeGroup->getTitle(), ':')): ?>
	<?php $currentHeading = substr($currentThemeGroup->getTitle(), 0, $pos) ?>
<?php endif; ?>
<?php foreach ($themeGroups as $themeGroup): ?>
	<?php if ($pos = stripos($themeGroup->getTitle(), ':')): ?>
		<?php $newHeading = substr($themeGroup->getTitle(), 0, $pos) ?>
		<?php if ($newHeading != $heading): ?>
			<?php $heading = $newHeading ?>
			<li class="inner <?php if ($currentThemeGroup && (stripos($currentThemeGroup->getTitle(), $heading)!==false)): ?>active<?php endif; ?>">
				<?php echo link_to($heading, '#', array('class' => 'category', 'onClick' => 'toggle(\'.'.$heading.'inner\', this);')) ?>
			</li>
		<?php endif; ?>
		<?php $body = substr($themeGroup->getTitle(), $pos + 1) ?>
			<?php if ($heading==$currentHeading): ?>
				<?php $style = 'display: block;' ?>
			<?php else: ?>
				<?php $style = 'display: none;' ?>
			<?php endif; ?>
		<li class="inner2 <?php echo $heading ?>inner <?php if ($module=='themeGroup' && $action=='read' && $id==$themeGroup->getId()): ?>active<?php endif; ?>" style="<?php echo $style ?>">
			<?php echo link_to($body, '/themeGroup/read?id='.$themeGroup->getId()) ?>
		</li>
	<?php else: ?>
		<li class="inner <?php if ($module=='themeGroup' && $action=='read' && $id==$themeGroup->getId()): ?>active<?php endif; ?>">
			<?php echo link_to($themeGroup->getTitle(), '/themeGroup/read?id='.$themeGroup->getId()) ?>
		</li>
	<?php endif; ?>
<?php endforeach; ?>
<?php endif; ?>
<li <?php if ($module=='application'): ?>class="active"<?php endif; ?> >
	<?php echo link_to('Applications', '/application/index') ?>
</li>
<li <?php if ($module=='module' && $application_id=='4'): ?>class="active"<?php endif; ?> >
	<?php echo link_to('E Modules', '/module/index?application_id=4') ?>
</li>
<?php if ($sf_user->hasCredential('admin')): ?>
	<li <?php if ($module=='user'): ?>class="active"<?php endif; ?> >
		<?php echo link_to('Users', '/user/index') ?>
	</li>
<?php elseif ($sf_user->hasCredential('user')): ?>
	<li <?php if ($module=='user'): ?>class="active"<?php endif; ?> >
		<?php echo link_to('My Settings', '/user/read?id='.$sf_user->getId()) ?>
	</li>
<?php endif; ?>
<?php if ($sf_user->hasCredential('user')): ?>
	<li>
		<?php echo link_to('Logout', '/user/logout') ?>
	</li>
<?php else: ?>
	<li <?php if ($module=='user' && $action=='login'): ?>class="active"<?php endif; ?> >
		<?php echo link_to('Login', '/user/login') ?>
	</li>
<?php endif; ?>