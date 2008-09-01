<?php

class LoginForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'name'       => new sfWidgetFormInput(),
			'password'   => new sfWidgetFormInputPassword(),
			'remember'   => new sfWidgetFormInputCheckbox(array(), array('value' => 1)),
		));
		$this->widgetSchema->setLabels(array(
			'name'        => 'Username',
			'password'    => 'Password',
			'remember'    => 'Remember Me',
		));
		$this->widgetSchema->setNameFormat('user[%s]');
	
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'name'   => new sfValidatorPass(),
			'password'   => new sfValidatorPass(),
			'remember'   => new sfValidatorPass(),
			)));
		$this->validatorSchema->setPostValidator(new validatorLogin(array('username' => 'name', 'password' => 'password'),
														array('invalid' => 'Invalid Username or Password')
													));
	}
}
