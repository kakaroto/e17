<?php
##IP_CHECK##
require_once(dirname(__FILE__).'/../exchange/config/ProjectConfiguration.class.php');

$configuration = ProjectConfiguration::getApplicationConfiguration('exchange', 'dev', true);
sfContext::createInstance($configuration)->dispatch();
