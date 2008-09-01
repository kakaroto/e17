<?php

class applicationComponents extends exchangeComponents
{
	public function executeTable()
	{
		$q = new Doctrine_Query();
		$q = $q->select('a.*')
				->from('Application a');
		$this->showcase = $this->getRequest()->getParameter('showcase', $this->showcase);
		$this->user_id = $this->getRequest()->getParameter('user_id', $this->user_id);
		$this->search = $this->getRequest()->getParameter('search', $this->search);
		if ($this->showcase)
			$q = $q->addWhere('a.showcase = ?', array(true));
		if ($this->user_id)
			$q = $q->addWhere('a.user_id = ?', array($this->user_id));
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('a.approved = ? or a.user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('a.approved = ?', array(true));
		}
		if ($this->search)
			$q = $q->addWhere('(a.name LIKE ? or a.description LIKE ?)', array('%'.$this->search.'%', '%'.$this->search.'%'));
			
		$sort = $this->getRequest()->getCookie('application_sort');
		if (!$sort)
			$sort = 'updated';
		$order = $this->getRequest()->getCookie('application_order');
		if (!$order)
			$order = 'DESC';
		$limit = $this->getRequest()->getCookie('application_limit');
		if (!$limit)
			$limit = '5';
		$sort = $this->getRequest()->getParameter('sort', $sort);
		$order = $this->getRequest()->getParameter('order', $order);
		$limit = $this->getRequest()->getParameter('limit', $limit);
		$page = $this->getRequest()->getParameter('page', 1);
		$this->getResponse()->setCookie('application_sort', $sort, time()+60*60*24*30, '/');
		$this->getResponse()->setCookie('application_order', $order, time()+60*60*24*30, '/');
		$this->getResponse()->setCookie('application_limit', $limit, time()+60*60*24*30, '/');
		$this->sort_val = $sort;
		$this->order = $order;
		$this->limit = $limit;
		$this->form = new SortForm(array('sort' => $sort, 'order' => $order, 'limit' => $limit));
		
		if ($sort=='rating')
			$q = $q->orderby('a.rating '.$order);
		elseif ($sort=='downloads')
			$q = $q->orderby('a.downloads '.$order);
		elseif ($sort=='created')
			$q = $q->orderby('a.created_at '.$order);
		elseif ($sort=='alphabetical')
			$q = $q->orderby('a.name '.$order);
		elseif ($sort=='owner')
			$q = $q->innerJoin('a.User u')->orderby('u.name '.$order);
		elseif ($sort=='updated')
			$q = $q->orderby('a.updated_at '.$order);
		$count = $q->count();
		if ($count <= (($page - 1) * $limit))
			$page = ceil($count / $limit);
		if ($page < 1)
			$page = 1;
		$q->limit($limit);
		$q->offset(($page - 1) * $limit);
		$this->applications = $q->execute();
		
		$args = array();
		if ($this->user_id)
			$args['user_id'] = $this->user_id;
		if ($this->search)
			$args['search'] = $this->search;
		$this->allApplications = '/application/index?'.http_build_query($args);
		if ($this->showcase)
			$args['showcase'] = $this->showcase;
		$this->prevPage = $this->nextPage = false;
		if ($page > 1) {
			$args['page'] = $page - 1;
			$this->prevPage = '/application/index?'.http_build_query($args);
		}
		if (($page * $limit) < $count) {
			$args['page'] = $page + 1;
			$this->nextPage = '/application/index?'.http_build_query($args);
		}
		$this->range = (($page - 1) * $limit + 1).' to '.(($page - 1) * $limit + $this->applications->count()).' of '.$count;
	}
}
