<?php

class ApplicationThemeGroupForm extends sfForm
{
	public function configure()
	{
		$application_options = Application::getAsOptions();
		$themeGroup_options = ThemeGroup::getAsOptions($this->defaults['application_id']);
		$this->setWidgets(array(
			'application_id' => new sfWidgetFormSelect(array('choices' => $application_options)),
			'theme_group_id' => new sfWidgetFormSelect(array('choices' => $themeGroup_options)),
		));
		$this->widgetSchema->setLabels(array(
			'application_id' => 'Application',
			'theme_group_id' => 'Theme Group',
		));
		$this->widgetSchema->setNameFormat('applicationThemeGroup[%s]');
		
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
	    
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'application_id' => new sfValidatorChoice(array('choices' => array_keys($application_options))),
			'theme_group_id' => new sfValidatorChoice(array('choices' => array_keys($themeGroup_options))),
		)));
	}
}