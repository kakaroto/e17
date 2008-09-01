<?php

class UserForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'id'         => new sfWidgetFormInputHidden(),
			'name'       => new sfWidgetFormInput(),
			'password'   => new sfWidgetFormInputPassword(),
			'password2'  => new sfWidgetFormInputPassword(),
			'email'      => new sfWidgetFormInput(),
			'role'       => new sfWidgetFormSelect(array('choices' => User::getRoleOptions())),
		));
		$this->widgetSchema->setLabels(array(
			'name'        => 'Username',
			'password'    => 'Password',
			'password2'   => 'Password Again',
			'email'       => 'Email',
			'role'        => 'Role',
		));
		$this->widgetSchema->setNameFormat('user[%s]');
		
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
		
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'name'   => new sfValidatorAnd(array(
							new sfValidatorString(array('required' => true, 'min_length' => 4),
									array('min_length' => 'Name must be at least 4 characters long')
								),
							new sfValidatorRegex(array('pattern' => '#^[a-zA-Z0-9-_]*$#'),
									array('invalid' => 'Name must be Alphanumeric')
								)
							),
							array('required'   => true),
							array('required'   => 'Username is required')
						),
			'email'  => new sfValidatorAnd(array(
							new sfValidatorString(array('required' => true, 'min_length' => 3),
									array('min_length' => 'Email must be at least 3 characters long')
								),
							new sfValidatorEmail(array(), array('invalid' => 'Please enter a valid email address')),
							new sfValidatorDoctrineUnique(array('model' => 'User', 'column' => 'email', 'primary_key' => 'id'))
							),
							array('required'   => true),
							array('required'   => 'Email is required')
						),
			'role'   => new sfValidatorPass(),
			'password'   => new sfValidatorString(array('required' => false, 'min_length' => 6),
									array('min_length' => 'Password must be at least 6 characters long')),
			'password2'   => new sfValidatorString(array('required' => false, 'min_length' => 6),
									array('min_length' => 'Password must be at least 6 characters long')),
			'id'   => new sfValidatorPass(),
		)));
		$this->validatorSchema->setPostValidator(new sfValidatorAnd(array(new sfValidatorSchemaCompare('password', '==', 'password2',
														array(), array('invalid' => 'Passwords do not match')),
													new sfValidatorDoctrineUnique(array('model' => 'User', 'column' => 'name', 'primary_key' => 'id'),
														array('invalid' => 'This Username is already in use')
													),
													new sfValidatorDoctrineUnique(array('model' => 'User', 'column' => 'email', 'primary_key' => 'id'),
														array('invalid' => 'This Email is already in use')
													))));
	}
}