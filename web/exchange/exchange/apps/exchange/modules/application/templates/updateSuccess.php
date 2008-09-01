<?php slot('title') ?><?php if ($application->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Application<?php end_slot() ?>
<?php if ($application->getId()): ?>
<form method="post" action="/application/edit/<?php echo $application->getId() ?>" enctype="multipart/form-data">
<?php else: ?>
<form method="post" action="/application/create" enctype="multipart/form-data">
<?php endif; ?>
<table>
<tbody>
<tr>
	<td colspan="2"><h1><?php if ($application->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> Application</h1></td>
</tr>
<?php echo $form ?>
<tr>
	<th></th>
	<td><input type="submit" value="Save" /></td>
</tr>
</tbody>
</table>
</form>