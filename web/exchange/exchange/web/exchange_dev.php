<?php
##IP_CHECK##
require_once(dirname(__FILE__).'/../config/ProjectConfiguration.class.php');

$configuration = ProjectConfiguration::getApplicationConfiguration('exchange', 'dev', true);
sfContext::createInstance($configuration)->dispatch();
