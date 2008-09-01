<?php

class ForgotForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'name'       => new sfWidgetFormInput(),
			'email'      => new sfWidgetFormInput(),
		));
		$this->widgetSchema->setLabels(array(
			'name'        => 'Username',
			'email'       => 'Email',
		));
		$this->widgetSchema->setNameFormat('user[%s]');
	
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'name'   => new sfValidatorPass(),
			'email'  => new sfValidatorEmail(array(), array('invalid' => 'Please enter a valid email address')),
			)));
	}
}
