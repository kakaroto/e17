<?php slot('title') ?><?php if ($themeGroup->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Theme Group<?php end_slot() ?>
<?php if ($themeGroup->getId()): ?>
<form method="post" action="/themeGroup/edit/<?php echo $themeGroup->getId() ?>">
<?php else: ?>
<form method="post" action="/themeGroup/create">
<?php endif; ?>
<table>
<tbody>
<tr>
	<td colspan="2"><h1><?php if ($themeGroup->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Theme Group</h1></td>
</tr>
<tr>
	<th>Name: </th>
	<td><?php echo $themeGroup->getName() ?></td>
</tr>
<?php echo $form ?>
<tr>
	<th></th>
	<td><input type="submit" value="Save" /></td>
</tr>
</tbody>
</table>
</form>