<?php slot('title') ?>Login<?php end_slot() ?>
<form method="post" action="/user/login">
<table>
<tbody>
<tr>
	<td colspan="2"><h1>Login</h1></td>
</tr>
<?php echo $form ?>
<tr>
	<th></th>
	<td><input type="submit" value="Login" /></td>
</tr>
<tr>
	<td colspan="2">
	<?php echo link_to('Register', '/user/update') ?> | <?php echo link_to('Forgot Password', '/user/forgot') ?> 
	</td>
</tr>
</tbody>
</table>
</form>