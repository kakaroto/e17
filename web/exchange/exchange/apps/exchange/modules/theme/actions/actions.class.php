<?php

class themeActions extends exchangeActions
{
	public function executeApprove()
	{
		$q = new Doctrine_Query();
		$this->theme = $q->select('t.*')
						->from('Theme t')
						->where('id = ?', array($this->getRequestParameter('id')))
						->fetchOne();
		$this->forward404Unless($this->theme);
		$this->theme->setApproved(!$this->theme->getApproved());
		$this->theme->save();
		return $this->redirect($this->getRequest()->getReferer()); 
	}

	public function executeDownload()
	{
		$q = new Doctrine_Query();
		$q = $q->select('t.*')
						->from('Theme t')
						->addWhere('id = ?', array($this->getRequestParameter('id')));
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('approved = ? or user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('approved = ?', array(true));
		}
		$this->theme = $q->fetchOne();
		$this->forward404Unless($this->theme);
		$this->theme->setDownloads($this->theme->getDownloads() + 1);
		$this->theme->save();
		$this->redirectRaw($this->theme->getUrlPath().$this->theme->getFileName());
	}
	
	public function executeIndex()
	{
		$this->user = $this->application = $this->madule = $this->theme_group = null;
		$this->user_id = $this->getRequest()->getParameter('user_id');
		if ($this->user_id)
			$this->user = Doctrine::getTable('User')->find($this->user_id);
		$this->application_id = $this->getRequest()->getParameter('application_id');
		if ($this->application_id)
			$this->application = Doctrine::getTable('Application')->find($this->application_id);
		$this->module_id = $this->getRequest()->getParameter('module_id');
		if ($this->module_id)
			$this->madule = Doctrine::getTable('Madule')->find($this->module_id);
		$this->theme_group_id = $this->getRequest()->getParameter('theme_group_id');
		if ($this->theme_group_id)
			$this->theme_group = Doctrine::getTable('ThemeGroup')->find($this->theme_group_id);
		$this->search = $this->getRequest()->getParameter('search');
	}
	
	public function executeRead()
	{
		$q = new Doctrine_Query();
		$q = $q->select('t.*')
						->from('Theme t')
						->addWhere('id = ?', array($this->getRequestParameter('id')));
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('approved = ? or user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('approved = ?', array(true));
		}
		$this->theme = $q->fetchOne();
		$this->forward404Unless($this->theme);
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$q = $q->select('t.*')
					->from('Theme t');
			$q = $q->addWhere('id = ?', array($id));
			if (!$this->getUser()->hasCredential('admin'))
				$q = $q->addWhere('user_id = ?', array($this->getUser()->getId()));
			$this->theme = $q->fetchOne();
			$this->forward404Unless($this->theme);
			$comments = $this->theme->getComments();
			foreach ($comments as $comment)
				$comment->delete();
			$ratings = $this->theme->getRatings();
			foreach ($ratings as $rating)
				$rating->delete();
			$themeGroups = $this->theme->getThemeThemeGroups();
			foreach ($themeGroups as $themeGroup)
				$themeGroup->delete();
			$this->theme->delete();
		}
		if (strstr($this->getRequest()->getReferer(), 'show') || strstr($this->getRequest()->getReferer(), 'read'))
			return $this->redirect('/theme/index');
		else 
			return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeShowcase()
	{
		$q = new Doctrine_Query();
		$this->theme = $q->select('t.*')
						->from('Theme t')
						->where('id = ?', array($this->getRequestParameter('id')))
						->fetchOne();
		$this->forward404Unless($this->theme);
		$this->theme->setShowcase(!$this->theme->getShowcase());
		$this->theme->save();
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeSort()
	{
		if ($this->getRequestParameter('sort'))
			$this->getResponse()->setCookie('theme_sort', $this->getRequestParameter('sort'), time()+60*60*24*30, '/');
		if ($this->getRequestParameter('order'))
			$this->getResponse()->setCookie('theme_order', $this->getRequestParameter('order'), time()+60*60*24*30, '/');
		if ($this->getRequestParameter('limit'))
			$this->getResponse()->setCookie('theme_limit', $this->getRequestParameter('limit'), time()+60*60*24*30, '/');
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('theme'), $request->getFiles('theme'));
			if ($this->form->isValid())
			{
				$theme = Theme::update($this->form->getValues(), $this->getUser()->getUser());
				$this->redirect('/theme/read?id='.$theme->getId());
			}
		}
	}
	
	public function prepareUpdate()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$q = $q->select('t.*')
					->from('Theme t');
			$q = $q->addWhere('id = ?', array($id));
			if (!$this->getUser()->hasCredential('admin'))
				$q = $q->addWhere('user_id = ?', array($this->getUser()->getId()));
			$this->theme = $q->fetchOne();
			$this->forwardUnsecureUnless($this->theme);
		} else
			$this->theme = new Theme();
		$this->form = new ThemeForm(array(	'id' => $this->theme->getId(),
											'name' => $this->theme->getName(),
											'description' => $this->theme->getDescription()));
	}
}
