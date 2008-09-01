<?php slot('title') ?><?php if ($madule->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Module<?php end_slot() ?>
<?php if ($madule->getId()): ?>
<form method="post" action="/module/edit/<?php echo $madule->getId() ?>" enctype="multipart/form-data">
<?php else: ?>
<form method="post" action="/module/create" enctype="multipart/form-data">
<?php endif; ?>
<table>
<tbody>
<tr>
	<td colspan="2"><h1><?php if ($madule->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Module</h1></td>
</tr>
<?php echo $form ?>
<tr>
	<th></th>
	<td><input type="submit" value="Save" /></td>
</tr>
</tbody>
</table>
</form>