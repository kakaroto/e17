<?php

class moduleActions extends exchangeActions
{
	public function executeApprove()
	{
		$q = new Doctrine_Query();
		$this->madule = $q->select('m.*')
						->from('Madule m')
						->where('id = ?', array($this->getRequestParameter('id')))
						->fetchOne();
		$this->forward404Unless($this->madule);
		$this->madule->setApproved(!$this->madule->getApproved());
		$this->madule->save();
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeIndex()
	{
		$this->user = $this->application = null;
		$this->user_id = $this->getRequest()->getParameter('user_id');
		if ($this->user_id)
			$this->user = Doctrine::getTable('User')->find($this->user_id);
		$this->application_id = $this->getRequest()->getParameter('application_id');
		if ($this->application_id)
			$this->application = Doctrine::getTable('Application')->find($this->application_id);
		$this->search = $this->getRequest()->getParameter('search');
	}
	
	public function executeRead()
	{
		$q = new Doctrine_Query();
		$q = $q->select('m.*')
						->from('Madule m')
						->addWhere('id = ?', array($this->getRequestParameter('id')));
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('approved = ? or user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('approved = ?', array(true));
		}
		$this->madule = $q->fetchOne();
		$this->forward404Unless($this->madule);
		$q = new Doctrine_Query();
		$q = $q->select('m.*')
						->from('MaduleThemeGroup m')
						->addWhere('madule_id = ?', array($this->madule->getId()));
		$this->hasThemes = $q->count();
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$q = $q->select('m.*')
					->from('Madule m');
			$q = $q->addWhere('id = ?', array($id));
			if (!$this->getUser()->hasCredential('admin'))
				$q = $q->addWhere('user_id = ?', array($this->getUser()->getId()));
			$this->madule = $q->fetchOne();
			$this->forward404Unless($this->madule);
			$comments = $this->madule->getComments();
			foreach ($comments as $comment)
				$comment->delete();
			$ratings = $this->madule->getRatings();
			foreach ($ratings as $rating)
				$rating->delete();
			$this->madule->delete();
		}
		if (strstr($this->getRequest()->getReferer(), 'show') || strstr($this->getRequest()->getReferer(), 'read'))
			return $this->redirect('/module/index');
		else 
			return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeShowcase()
	{
		$q = new Doctrine_Query();
		$this->madule = $q->select('m.*')
						->from('Madule m')
						->where('id = ?', array($this->getRequestParameter('id')))
						->fetchOne();
		$this->forward404Unless($this->madule);
		$this->madule->setShowcase(!$this->madule->getShowcase());
		$this->madule->save();
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeSort()
	{
		if ($this->getRequestParameter('sort'))
			$this->getResponse()->setCookie('module_sort', $this->getRequestParameter('sort'), time()+60*60*24*30, '/');
		if ($this->getRequestParameter('order'))
			$this->getResponse()->setCookie('module_order', $this->getRequestParameter('order'), time()+60*60*24*30, '/');
		if ($this->getRequestParameter('limit'))
			$this->getResponse()->setCookie('module_limit', $this->getRequestParameter('limit'), time()+60*60*24*30, '/');
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('madule'), $request->getFiles('madule'));
			if ($this->form->isValid())
			{
				$module = Madule::update($this->form->getValues(), $this->getUser()->getUser());
				$this->redirect('/module/read?id='.$module->getId());
			}
		}
	}
	
	public function prepareUpdate()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$q = $q->select('m.*')
					->from('Madule m');
			$q = $q->addWhere('id = ?', array($id));
			if (!$this->getUser()->hasCredential('admin'))
				$q = $q->addWhere('user_id = ?', array($this->getUser()->getId()));
			$this->madule = $q->fetchOne();
			$this->forwardUnsecureUnless($this->madule);
		} else {
			$this->madule = new Madule();
			if ($application_id = $this->getRequestParameter('application_id'))
				$this->madule->setApplicationId($application_id);
		}
		$this->form = new ModuleForm(array(	'id' => $this->madule->getId(),
											'name' => $this->madule->getName(),
											'description' => $this->madule->getDescription(),
											'source_url' => $this->madule->getSourceUrl(),
											'application_id' => $this->madule->getApplicationId()));
	}
}
