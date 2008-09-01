<?php

class commentComponents extends exchangeComponents
{
	public function executeTable()
	{
		$q = new Doctrine_Query();
		$q = $q->select('c.*')
				->from('Comment c');
		$this->application_id = $this->getRequest()->getParameter('application_id', $this->application_id);
		$this->module_id = $this->getRequest()->getParameter('module_id', $this->module_id);
		$this->theme_id = $this->getRequest()->getParameter('theme_id', $this->theme_id);
		$this->user_id = $this->getRequest()->getParameter('user_id', $this->user_id);
		if ($this->application_id)
			$q = $q->addWhere('application_id = ?', array($this->application_id));
		if ($this->module_id)
			$q = $q->addWhere('madule_id = ?', array($this->module_id));
		if ($this->theme_id)
			$q = $q->addWhere('theme_id = ?', array($this->theme_id));
		elseif ($this->user_id)
			$q = $q->addWhere('user_id = ?', array($this->user_id));
		
		$limit = $this->getRequest()->getCookie('comment_limit');
		if (!$limit)
			$limit = '5';
		$limit = $this->getRequest()->getParameter('limit', $limit);
		$page = $this->getRequest()->getParameter('page', 1);
		$this->getResponse()->setCookie('comment_limit', $limit, time()+60*60*24*30, '/');
		$this->limit = $limit;
		$this->sortForm = new SortForm(array('limit' => $limit));
		$q = $q->orderby('c.updated_at ASC');
		$count = $q->count();
		if ($count <= (($page - 1) * $limit))
			$page = ceil($count / $limit);
		if ($page < 1)
			$page = 1;
		$q->limit($limit);
		$q->offset(($page - 1) * $limit);
		$this->comments = $q->execute();
		
		$args = array();
		if ($this->user_id)
			$args['user_id'] = $this->user_id;
		if ($this->application_id)
			$args['application_id'] = $this->application_id;
		if ($this->module_id)
			$args['module_id'] = $this->module_id;
		if ($this->theme_id)
			$args['theme_id'] = $this->theme_id;
		$this->prevPage = $this->nextPage = false;
		if ($page > 1) {
			$args['page'] = $page - 1;
			$this->prevPage = '/comment/index?'.http_build_query($args);
		}
		if (($page * $limit) < $count) {
			$args['page'] = $page + 1;
			$this->nextPage = '/comment/index?'.http_build_query($args);
		}
		$this->range = (($page - 1) * $limit + 1).' to '.(($page - 1) * $limit + $this->comments->count()).' of '.$count;
		$this->form = new CommentForm(array('application_id' => $this->application_id, 'module_id' => $this->module_id, 'theme_id' => $this->theme_id));
	}
}
