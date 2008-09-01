<?php

class applicationActions extends exchangeActions
{
	public function executeApprove()
	{
		$q = new Doctrine_Query();
		$this->application = $q->select('a.*')
						->from('Application a')
						->where('id = ?', array($this->getRequestParameter('id')))
						->fetchOne();
		$this->forward404Unless($this->application);
		$this->application->setApproved(!$this->application->getApproved());
		$this->application->save();
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeIndex()
	{
		$this->user = null;
		$this->user_id = $this->getRequest()->getParameter('user_id');
		if ($this->user_id)
			$this->user = Doctrine::getTable('User')->find($this->user_id);
		$this->search = $this->getRequest()->getParameter('search');
	}
	
	public function executeRead()
	{
		$q = new Doctrine_Query();
		$q = $q->select('a.*')
						->from('Application a')
						->addWhere('id = ?', array($this->getRequestParameter('id')));
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('approved = ? or user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('approved = ?', array(true));
		}
		$this->application = $q->fetchOne();
		$this->forward404Unless($this->application);
		$q = new Doctrine_Query();
		$q = $q->select('m.*')
						->from('Madule m')
						->addWhere('application_id = ?', array($this->application->getId()));
		$this->hasModules = $q->count();
		$q = new Doctrine_Query();
		$q = $q->select('a.*')
						->from('ApplicationThemeGroup a')
						->addWhere('application_id = ?', array($this->application->getId()));
		$this->hasThemes = $q->count();
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$q = $q->select('a.*')
					->from('Application a');
			$q = $q->addWhere('id = ?', array($id));
			if (!$this->getUser()->hasCredential('admin'))
				$q = $q->addWhere('user_id = ?', array($this->getUser()->getId()));
			$this->application = $q->fetchOne();
			$this->forward404Unless($this->application);
			$comments = $this->application->getComments();
			foreach ($comments as $comment)
				$comment->delete();
			$ratings = $this->application->getRatings();
			foreach ($ratings as $rating)
				$rating->delete();
			$this->application->delete();
		}
		if (strstr($this->getRequest()->getReferer(), 'show') || strstr($this->getRequest()->getReferer(), 'read'))
			return $this->redirect('/application/index');
		else 
			return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeShowcase()
	{
		$q = new Doctrine_Query();
		$this->application = $q->select('a.*')
						->from('Application a')
						->where('id = ?', array($this->getRequestParameter('id')))
						->fetchOne();
		$this->forward404Unless($this->application);
		$this->application->setShowcase(!$this->application->getShowcase());
		$this->application->save();
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeSort()
	{
		if ($this->getRequestParameter('sort'))
			$this->getResponse()->setCookie('application_sort', $this->getRequestParameter('sort'), time()+60*60*24*30, '/');
		if ($this->getRequestParameter('order'))
			$this->getResponse()->setCookie('application_order', $this->getRequestParameter('order'), time()+60*60*24*30, '/');
		if ($this->getRequestParameter('limit'))
			$this->getResponse()->setCookie('application_limit', $this->getRequestParameter('limit'), time()+60*60*24*30, '/');
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('application'), $request->getFiles('application'));
			if ($this->form->isValid())
			{
				$application = Application::update($this->form->getValues(), $this->getUser()->getUser());
				$this->redirect('/application/read?id='.$application->getId());
			}
		}
	}
	
	public function prepareUpdate()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$q = $q->select('a.*')
					->from('Application a');
			$q = $q->addWhere('id = ?', array($id));
			if (!$this->getUser()->hasCredential('admin'))
				$q = $q->addWhere('user_id = ?', array($this->getUser()->getId()));
			$this->application = $q->fetchOne();
			$this->forwardUnsecureUnless($this->application);
		} else
			$this->application = new Application();
		$this->form = new ApplicationForm(array('id' => $this->application->getId(),
												'name' => $this->application->getName(),
												'description' => $this->application->getDescription(),
												'source_url' => $this->application->getSourceUrl()));
	}
}
