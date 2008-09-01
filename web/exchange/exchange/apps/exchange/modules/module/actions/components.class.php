<?php

class moduleComponents extends exchangeComponents
{
	public function executeTable()
	{
		$q = new Doctrine_Query();
		$q = $q->select('m.*')
				->from('Madule m');
		$this->showcase = $this->getRequest()->getParameter('showcase', $this->showcase);
		$this->user_id = $this->getRequest()->getParameter('user_id', $this->user_id);
		$this->application_id = $this->getRequest()->getParameter('application_id', $this->application_id);
		$this->search = $this->getRequest()->getParameter('search', $this->search);
		if ($this->showcase)
			$q = $q->addWhere('m.showcase = ?', array(true));
		if ($this->user_id)
			$q = $q->addWhere('m.user_id = ?', array($this->user_id));
		if ($this->application_id)
			$q = $q->addWhere('m.application_id = ?', array($this->application_id));
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('m.approved = ? or m.user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('m.approved = ?', array(true));
		}
		if ($this->search)
			$q = $q->addWhere('(m.name LIKE ? or m.description LIKE ?)', array('%'.$this->search.'%', '%'.$this->search.'%'));
			
		$sort = $this->getRequest()->getCookie('module_sort');
		if (!$sort)
			$sort = 'updated';
		$order = $this->getRequest()->getCookie('module_order');
		if (!$order)
			$order = 'DESC';
		$limit = $this->getRequest()->getCookie('module_limit');
		if (!$limit)
			$limit = '5';
		$sort = $this->getRequest()->getParameter('sort', $sort);
		$order = $this->getRequest()->getParameter('order', $order);
		$limit = $this->getRequest()->getParameter('limit', $limit);
		$page = $this->getRequest()->getParameter('page', 1);
		$this->getResponse()->setCookie('module_sort', $sort, time()+60*60*24*30, '/');
		$this->getResponse()->setCookie('module_order', $order, time()+60*60*24*30, '/');
		$this->getResponse()->setCookie('module_limit', $limit, time()+60*60*24*30, '/');
		$this->sort_val = $sort;
		$this->order = $order;
		$this->limit = $limit;
		$this->form = new SortForm(array('sort' => $sort, 'order' => $order, 'limit' => $limit));
		
		if ($sort=='rating')
			$q = $q->orderby('m.rating '.$order);
		elseif ($sort=='downloads')
			$q = $q->orderby('m.downloads '.$order);
		elseif ($sort=='created')
			$q = $q->orderby('m.created_at '.$order);
		elseif ($sort=='alphabetical')
			$q = $q->orderby('m.name '.$order);
		elseif ($sort=='owner')
			$q = $q->innerJoin('m.User u')->orderby('u.name '.$order);
		elseif ($sort=='updated')
			$q = $q->orderby('m.updated_at '.$order);
		$count = $q->count();
		if ($count <= (($page - 1) * $limit))
			$page = ceil($count / $limit);
		if ($page < 1)
			$page = 1;
		$q->limit($limit);
		$q->offset(($page - 1) * $limit);
		$this->madules = $q->execute();
		
		$args = array();
		if ($this->user_id)
			$args['user_id'] = $this->user_id;
		if ($this->application_id)
			$args['application_id'] = $this->application_id;
		if ($this->search)
			$args['search'] = $this->search;
		$this->allModules = '/module/index?'.http_build_query($args);
		if ($this->showcase)
			$args['showcase'] = $this->showcase;
		$this->prevPage = $this->nextPage = false;
		if ($page > 1) {
			$args['page'] = $page - 1;
			$this->prevPage = '/module/index?'.http_build_query($args);
		}
		if (($page * $limit) < $count) {
			$args['page'] = $page + 1;
			$this->nextPage = '/module/index?'.http_build_query($args);
		}
		$this->range = (($page - 1) * $limit + 1).' to '.(($page - 1) * $limit + $this->madules->count()).' of '.$count;
	}
}
