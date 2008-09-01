<?php

class Tools {

	public static function get($key)
	{
		return sfConfig::get('app_'.$key);
	}
}
?>