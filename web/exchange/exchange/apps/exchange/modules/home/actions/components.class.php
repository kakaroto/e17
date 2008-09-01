<?php

class homeComponents extends exchangeComponents
{	
	public function executeNavigation($request)
	{
		$module = $this->getContext()->getModuleName();
		$action = $this->getContext()->getActionName();
		$this->module = $module;
		$this->action = $action;
		$this->type = $request->getParameter('type');
		$this->application_id = $request->getParameter('application_id');
		if ($module=='module' && ($action=='read' || $action=='update')) {
			$madule = Doctrine::getTable('Madule')->find($request->getParameter('id'));
			if ($madule)
				$this->application_id = $madule->getApplicationId();
		}
		$q = new Doctrine_Query();
		$this->themeGroups = $q->select('t.*')
					->from('ThemeGroup t')
					->where('t.known = ?', array(true))
					->orderby('t.title ASC')
					->execute();
		$this->id = $this->getRequestParameter('id');
		$this->currentThemeGroup = null;
		if ($this->module=='themeGroup')
			$this->currentThemeGroup = $q->select('t.*')
					->from('ThemeGroup t')
					->where('t.id = ? and t.known = ?', array($this->id, true))
					->orderby('t.title ASC')
					->fetchOne();
	}
}