<?php

class ModuleThemeGroupForm extends sfForm
{
	public function configure()
	{
		$module_options = Madule::getAsOptions();
		$themeGroup_options = ThemeGroup::getAsOptions(null, $this->defaults['module_id']);
		$this->setWidgets(array(
			'module_id' => new sfWidgetFormSelect(array('choices' => $module_options)),
			'theme_group_id' => new sfWidgetFormSelect(array('choices' => $themeGroup_options)),
		));
		$this->widgetSchema->setLabels(array(
			'module_id' => 'Module',
			'theme_group_id' => 'Theme Group',
		));
		$this->widgetSchema->setNameFormat('moduleThemeGroup[%s]');
		
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
	    
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'module_id' => new sfValidatorChoice(array('choices' => array_keys($module_options))),
			'theme_group_id' => new sfValidatorChoice(array('choices' => array_keys($themeGroup_options))),
		)));
	}
}