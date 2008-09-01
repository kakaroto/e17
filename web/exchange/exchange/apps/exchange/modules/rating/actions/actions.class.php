<?php

class ratingActions extends exchangeActions
{
	public function executeIndex()
	{
		
	}
	
	public function executeRate()
	{
		$params = explode('.', $this->getRequestParameter('object_id'));
		$type = $params[0];
		$object_id = $params[1];
		$value = $this->getRequestParameter('value');
		$rating = null;
		if ($type == Rating::APPLICATION) {
			$object = Doctrine::getTable('Application')->find($object_id);
			$q = new Doctrine_Query();
			$rating = $q->select('r.*')
						->from('Rating r')
						->where('application_id = ? and user_id = ?', 
							array($object_id, $this->getUser()->getId()))
						->fetchOne();
			if (!$rating)
			{
				$rating = new Rating();
				$rating->setUserId($this->getUser()->getId());
				$rating->setApplicationId($object_id);
			}
		} elseif ($type == Rating::MODULE) {
			$object = Doctrine::getTable('Madule')->find($object_id);
			$q = new Doctrine_Query();
			$rating = $q->select('r.*')
						->from('Rating r')
						->where('madule_id = ? and user_id = ?', 
							array($object_id, $this->getUser()->getId()))
						->fetchOne();
			if (!$rating)
			{
				$rating = new Rating();
				$rating->setUserId($this->getUser()->getId());
				$rating->setMaduleId($object_id);
			}
		} elseif ($type == Rating::THEME) {
			$object = Doctrine::getTable('Theme')->find($object_id);
			$q = new Doctrine_Query();
			$rating = $q->select('r.*')
						->from('Rating r')
						->where('theme_id = ? and user_id = ?', 
							array($object_id, $this->getUser()->getId()))
						->fetchOne();
			if (!$rating)
			{
				$rating = new Rating();
				$rating->setUserId($this->getUser()->getId());
				$rating->setThemeId($object_id);
			}
		}
		$this->forward404Unless($value <= 5);
		$this->forward404Unless($value >= 1);
		if ($rating) {
			$rating->setValue($value);
			$rating->save();
			$this->my_rating = $value;
			$this->rating = $object->getRating();
		}
	}

}
