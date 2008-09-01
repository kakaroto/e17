<?php

class themeGroupComponents extends exchangeComponents
{
	public function executeTable()
	{
		$q = new Doctrine_Query();
		$q = $q->select('t.*')
						->from('ThemeGroup t');
		if (!$this->application_id)
			$this->application_id = false;
		if (!$this->module_id)
			$this->module_id = false;
		if (!$this->theme_id)
			$this->theme_id = false;
		if (!$this->compact)
			$this->compact = false;
		$this->application_id = $this->getRequest()->getParameter('application_id', $this->application_id);
		$this->module_id = $this->getRequest()->getParameter('module_id', $this->module_id);
		$this->theme_id = $this->getRequest()->getParameter('theme_id', $this->theme_id);
		if ($this->application_id)
			$q = $q->innerJoin('t.ApplicationThemeGroups atg')
						->addWhere('atg.application_id = ?', array($this->application_id))
						->orderby('t.title ASC');
		elseif ($this->module_id)
			$q = $q->innerJoin('t.MaduleThemeGroups mtg')
						->addWhere('mtg.madule_id = ?', array($this->module_id))
						->orderby('t.title ASC');
		elseif ($this->theme_id)
			$q = $q->innerJoin('t.ThemeThemeGroups ttg')
						->addWhere('ttg.theme_id = ?', array($this->theme_id))
						->orderby('t.title ASC');
		else
			$q = $q->orderby('t.known DESC, t.title ASC, t.name ASC');
		if ($this->compact)
			$q = $q->addWhere('t.known = ?', array(true));
		if ($this->search)
			$q = $q->addWhere('(name LIKE ? or title LIKE ?)', array('%'.$this->search.'%', '%'.$this->search.'%'));
		$this->themeGroups = $q->execute();
		$args = array();
		if ($this->application_id)
			$args['application_id'] = $this->application_id;
		if ($this->module_id)
			$args['module_id'] = $this->module_id;
		if ($this->theme_id)
			$args['theme_id'] = $this->theme_id;
		$this->morePage = '/themeGroup/index?'.http_build_query($args);
	}
}
