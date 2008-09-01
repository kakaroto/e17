<?php

class themeComponents extends exchangeComponents
{
	public function executeTable()
	{
		$q = new Doctrine_Query();
		$q = $q->select('t.*')
				->from('Theme t');
		$this->showcase = $this->getRequest()->getParameter('showcase', $this->showcase);
		$this->user_id = $this->getRequest()->getParameter('user_id', $this->user_id);
		$this->application_id = $this->getRequest()->getParameter('application_id', $this->application_id);
		$this->module_id = $this->getRequest()->getParameter('module_id', $this->module_id);
		$this->theme_group_id = $this->getRequest()->getParameter('theme_group_id', $this->theme_group_id);
		$this->search = $this->getRequest()->getParameter('search', $this->search);
		if ($this->showcase)
			$q = $q->addWhere('t.showcase = ?', array(true));
		if ($this->user_id)
			$q = $q->addWhere('t.user_id = ?', array($this->user_id));
		if ($this->application_id) {
			$q = $q->addWhere('t.id IN (SELECT ttg.theme_id from ThemeThemeGroup ttg where ttg.theme_group_id IN ' .
										'(SELECT ag.theme_group_id FROM ApplicationThemeGroup ag where ag.application_id = ?))', array($this->application_id));
		} elseif ($this->module_id) {
			$q = $q->addWhere('t.id IN (SELECT ttg.theme_id from ThemeThemeGroup ttg where ttg.theme_group_id IN ' .
										'(SELECT mg.theme_group_id FROM MaduleThemeGroup mg where mg.madule_id = ?))', array($this->module_id));
		} elseif ($this->theme_group_id) {
			$ids = explode(' ', $this->theme_group_id);
			$where = '';
			foreach ($ids as $id) {
				$where .= 't.id IN (SELECT ttg.theme_id from ThemeThemeGroup ttg where ttg.theme_group_id = ?) and ';
			}
			$where = substr($where, 0, -5);
			$q = $q->addWhere($where, $ids);
		}
		if (!$this->getUser()->hasCredential('admin'))
		{
			if ($this->getUser()->hasCredential('user'))
				$q = $q->addWhere('t.approved = ? or t.user_id = ?', array(true, $this->getUser()->getId()));
			else
				$q = $q->addWhere('t.approved = ?', array(true));
		}
		if ($this->search)
			$q = $q->addWhere('(t.name LIKE ? or t.description LIKE ?)', array('%'.$this->search.'%', '%'.$this->search.'%'));
			
		$sort = $this->getRequest()->getCookie('theme_sort');
		if (!$sort)
			$sort = 'updated';
		$order = $this->getRequest()->getCookie('theme_order');
		if (!$order)
			$order = 'DESC';
		$limit = $this->getRequest()->getCookie('theme_limit');
		if (!$limit)
			$limit = '5';
		$sort = $this->getRequest()->getParameter('sort', $sort);
		$order = $this->getRequest()->getParameter('order', $order);
		$limit = $this->getRequest()->getParameter('limit', $limit);
		$page = $this->getRequest()->getParameter('page', 1);
		$this->getResponse()->setCookie('theme_sort', $sort, time()+60*60*24*30, '/');
		$this->getResponse()->setCookie('theme_order', $order, time()+60*60*24*30, '/');
		$this->getResponse()->setCookie('theme_limit', $limit, time()+60*60*24*30, '/');
		$this->sort_val = $sort;
		$this->order = $order;
		$this->limit = $limit;
		$this->form = new SortForm(array('sort' => $sort, 'order' => $order, 'limit' => $limit));
		
		if ($sort=='rating')
			$q = $q->orderby('t.rating '.$order);
		elseif ($sort=='downloads')
			$q = $q->orderby('t.downloads '.$order);
		elseif ($sort=='created')
			$q = $q->orderby('t.created_at '.$order);
		elseif ($sort=='alphabetical')
			$q = $q->orderby('t.name '.$order);
		elseif ($sort=='owner')
			$q = $q->innerJoin('t.User u')->orderby('u.name '.$order);
		elseif ($sort=='updated')
			$q = $q->orderby('t.updated_at '.$order);
		$count = $q->count();
		if ($count <= (($page - 1) * $limit))
			$page = ceil($count / $limit);
		if ($page < 1)
			$page = 1;
		$q->limit($limit);
		$q->offset(($page - 1) * $limit);
		$this->themes = $q->execute();
		
		$args = array();
		if ($this->user_id)
			$args['user_id'] = $this->user_id;
		if ($this->application_id)
			$args['application_id'] = $this->application_id;
		if ($this->module_id)
			$args['module_id'] = $this->module_id;
		if ($this->theme_group_id)
			$args['theme_group_id'] = $this->theme_group_id;
		if ($this->search)
			$args['search'] = $this->search;
		$this->allThemes = '/theme/index?'.http_build_query($args);
		if ($this->showcase)
			$args['showcase'] = $this->showcase;
		$this->prevPage = $this->nextPage = false;
		if ($page > 1) {
			$args['page'] = $page - 1;
			$this->prevPage = '/theme/index?'.http_build_query($args);
		}
		if (($page * $limit) < $count) {
			$args['page'] = $page + 1;
			$this->nextPage = '/theme/index?'.http_build_query($args);
		}
		$this->range = (($page - 1) * $limit + 1).' to '.(($page - 1) * $limit + $this->themes->count()).' of '.$count;
	}
}
