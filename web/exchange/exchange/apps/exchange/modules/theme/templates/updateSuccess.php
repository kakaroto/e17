<?php slot('title') ?><?php if ($theme->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Theme<?php end_slot() ?>
<?php if ($theme->getId()): ?>
<form method="post" action="/theme/edit/<?php echo $theme->getId() ?>" enctype="multipart/form-data">
<?php else: ?>
<form method="post" action="/theme/create" enctype="multipart/form-data">
<?php endif; ?>
<table>
<tbody>
<tr>
	<td colspan="2"><h1><?php if ($theme->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Theme</h1></td>
</tr>
<?php echo $form ?>
<tr>
	<th></th>
	<td><input type="submit" value="Save" /></td>
</tr>
</tbody>
</table>
</form>