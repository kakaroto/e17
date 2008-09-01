<?php

class ThemeGroupForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'title'       => new sfWidgetFormInput(),
			'known'       => new sfWidgetFormInputCheckbox(array(), array('value' => 1)),
		));
		$this->widgetSchema->setNameFormat('themeGroup[%s]');
		
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
		
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'title'   => new sfValidatorPass(),
			'known'  => new sfValidatorPass(),
		)));
	}
}