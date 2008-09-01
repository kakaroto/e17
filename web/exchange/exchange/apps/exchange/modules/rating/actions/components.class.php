<?php

class ratingComponents extends exchangeComponents
{
	public function executeRating()
	{
		if ($this->application_id)
			$this->object_id = Rating::APPLICATION.'.'.$this->application_id;
		if ($this->module_id)
			$this->object_id = Rating::MODULE.'.'.$this->module_id;
		if ($this->theme_id)
			$this->object_id = Rating::THEME.'.'.$this->theme_id;
	}
}
