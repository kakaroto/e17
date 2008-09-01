<?php

/*
 * This file is part of the symfony package.
 * (c) Fabien Potencier <fabien.potencier@symfony-project.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Doctrine form generator.
 *
 * This class generates a Doctrine forms.
 *
 * @package    symfony
 * @subpackage generator
 * @author     Jonathan H. Wage <jonwage@gmail.com>
 * @author     Fabien Potencier <fabien.potencier@symfony-project.com>
 * @version    SVN: $Id: sfDoctrineFormGenerator.class.php 8512 2008-04-17 18:06:12Z fabien $
 */
class sfDoctrineFormGenerator extends sfGenerator
{
  /**
   * Array of all the loaded models
   *
   * @var array
   */
  public $models = array();

  /**
   * Array of all plugin models
   *
   * @var array
   */
  public $pluginModels = array();

  /**
   * Initializes the current sfGenerator instance.
   *
   * @param sfGeneratorManager A sfGeneratorManager instance
   */
  public function initialize(sfGeneratorManager $generatorManager)
  {
    parent::initialize($generatorManager);

    $this->getPluginModels();
    $this->setGeneratorClass('sfDoctrineForm');
  }

  /**
   * Generates classes and templates in cache.
   *
   * @param array The parameters
   *
   * @return string The data to put in configuration cache
   */
  public function generate($params = array())
  {
    $this->params = $params;

    if (!isset($this->params['connection']))
    {
      throw new sfParseException('You must specify a "connection" parameter.');
    }

    if (!isset($this->params['model_dir_name']))
    {
      $this->params['model_dir_name'] = 'model';
    }

    if (!isset($this->params['form_dir_name']))
    {
      $this->params['form_dir_name'] = 'form';
    }

    $models = $this->loadModels();

    // create the project base class for all forms
    $file = sfConfig::get('sf_lib_dir').'/form/BaseFormDoctrine.class.php';
    if (!file_exists($file))
    {
      if (!is_dir(sfConfig::get('sf_lib_dir').'/form/doctrine'))
      {
        mkdir(sfConfig::get('sf_lib_dir').'/form/doctrine', 0777, true);
      }

      file_put_contents($file, $this->evalTemplate('sfDoctrineFormBaseTemplate.php'));
    }

    // create a form class for every Doctrine class
    foreach ($models as $model)
    {
      $this->table = Doctrine::getTable($model);
      $this->modelName = $model;

      if ($this->isPluginModel($model))
      {
        $baseDir = sfConfig::get('sf_plugins_dir') . '/' . $this->getPluginNameForModel($model) . '/lib/form/doctrine';
      } else {
        $baseDir = sfConfig::get('sf_lib_dir') . '/form/doctrine';
      }

      if (!is_dir($baseDir.'/base'))
      {
        mkdir($baseDir.'/base', 0777, true);
      }

      file_put_contents($baseDir.'/base/Base'.$model.'Form.class.php', $this->evalTemplate('sfDoctrineFormGeneratedTemplate.php'));
      if (!file_exists($classFile = $baseDir.'/'.$model.'Form.class.php'))
      {
        file_put_contents($classFile, $this->evalTemplate('sfDoctrineFormTemplate.php'));
      }
    }
  }

  /**
   * Get all the models which are a part of a plugin and the name of the plugin.
   * The array format is modelName => pluginName
   *
   * @return array $pluginModels
   */
  public function getPluginModels()
  {
    if (!$this->pluginModels)
    {
      $dirs = $this->generatorManager->getConfiguration()->getModelDirs();
      unset($dirs[0]);
      $dirs = array_values($dirs);

      $models = sfFinder::type('*.php')->in($dirs);
      foreach ($models as $path)
      {
        $info = pathinfo($path);
        $pluginName = explode(DIRECTORY_SEPARATOR, dirname(dirname(dirname($info['dirname']))));
        $pluginName = end($pluginName);

        $e = explode('.', $info['filename']);
        $modelName = substr($e[0], 6, strlen($e[0]));

        $this->pluginModels[$modelName] = $pluginName;
      }
    }

    return $this->pluginModels;
  }

  /**
   * Check to see if a model is part of a plugin
   *
   * @param string $modelName 
   * @return boolean $bool
   */
  public function isPluginModel($modelName)
  {
    return isset($this->pluginModels[$modelName]) ? true:false;
  }

  /**
   * Get the name of the plugin a model belongs to
   *
   * @param string $modelName 
   * @return string $pluginName
   */
  public function getPluginNameForModel($modelName)
  {
    if ($this->isPluginModel($modelName))
    {
      return $this->pluginModels[$modelName];
    } else {
      return false;
    }
  }

  /**
   * Returns an array of relations that represents a many to many relationship.
   *
   * A table is considered to be a m2m table if it has 2 foreign keys that are also primary keys.
   *
   * @return array An array of relations.
   */
  public function getManyToManyRelations()
  {
    $relations = array();
    foreach ($this->table->getRelations() as $relation)
    {
      if ($relation->getType() === Doctrine_Relation::MANY && isset($relation['refTable']))
      {
        $relations[] = $relation;
      }
    }
    return $relations;
  }

  /**
   * Returns PHP names for all foreign keys of the current table.
   *
   * This method does not returns foreign keys that are also primary keys.
   *
   * @return array An array composed of: 
   *                 * The foreign table PHP name
   *                 * The foreign key PHP name
   *                 * A Boolean to indicate whether the column is required or not
   *                 * A Boolean to indicate whether the column is a many to many relationship or not
   */
  public function getForeignKeyNames()
  {
    $names = array();
    foreach ($this->table->getRelations() as $relation)
    {
      if ($relation->getType() === Doctrine_Relation::ONE)
      {
        $foreignDef = $relation->getTable()->getDefinitionOf($relation->getForeignFieldName());
        $names[] = array($relation['table']->getOption('name'), $relation->getForeignFieldName(), $this->isColumnNotNull($relation->getForeignFieldName(), $foreignDef), false);
      }
    }

    foreach ($this->getManyToManyRelations() as $relation)
    {
      $names[] = array($relation['table']->getOption('name'), $relation['refTable']->getOption('name'), false, true);
    }

    return $names;
  }

  /**
   * Returns the first primary key column of the current table.
   *
   * @param ColumnMap A ColumnMap object
   */
  public function getPrimaryKey()
  {
    foreach (array_keys($this->table->getColumns()) as $name)
    {
      if ($this->isColumnPrimaryKey($name))
      {
        return $this->table->getDefinitionOf($name);
      }
    }
  }

  /**
   * Returns a sfWidgetForm class name for a given column.
   *
   * @param  ColumnMap A ColumnMap object
   *
   * @return string    The name of a subclass of sfWidgetForm
   */
  public function getWidgetClassForColumn($columnName)
  {
    $column = $this->table->getDefinitionOf($columnName);
    switch ($column['type'])
    {
      case 'boolean':
        $widgetSubclass = 'InputCheckbox';
        break;
      case 'glob':
      case 'clob':
        $widgetSubclass = 'Textarea';
        break;
      case 'date':
        $widgetSubclass = 'Date';
        break;
      case 'time':
        $widgetSubclass = 'Time';
        break;
      case 'timestamp':
        $widgetSubclass = 'DateTime';
        break;
      default:
        $widgetSubclass = 'Input';
    }


    if ($this->isColumnPrimaryKey($columnName))
    {
      $widgetSubclass = 'InputHidden';
    }
    else if ($this->isColumnForeignKey($columnName))
    {
      $widgetSubclass = 'DoctrineSelect';
    }

    return sprintf('sfWidgetForm%s', $widgetSubclass);
  }

  /**
   * Check if a column is a foreign key
   *
   * @param array $column 
   * @return boolean $bool
   */
  public function isColumnForeignKey($name)
  {
    if ($this->isColumnPrimaryKey($name))
    {
      return false;
    }

    foreach ($this->table->getRelations() as $relation)
    {
      if ($relation['local'] == $name)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Get the foreign table object for a column
   *
   * @param string $name 
   * @param string $column 
   * @return Doctrine_Table $table
   */
  public function getForeignTable($name)
  {
    foreach ($this->table->getRelations() as $relation)
    {
      if ($relation['local'] == $name)
      {
        return $relation['table'];
      }
    }
    return false;
  }

  /**
   * Check if a column is a primary key
   *
   * @param string $name
   * @return boolean $bool
   */
  public function isColumnPrimaryKey($name)
  {
    $column = $this->table->getDefinitionOf($name);
    return (isset($column['primary']) && $column['primary']);
  }

  /**
   * Check if a column is not null
   *
   * @param array $column 
   * @return boolean $bool
   */
  public function isColumnNotNull($name)
  {
    $column = $this->table->getDefinitionOf($name);
    return (isset($column['notnull']) && $column['notnull']);
  }

  /**
   * Returns a PHP string representing options to pass to a widget for a given column.
   *
   * @param  ColumnMap A ColumnMap object
   *
   * @return string    The options to pass to the widget as a PHP string
   */
  public function getWidgetOptionsForColumn($name)
  {
    $column = $this->table->getDefinitionOf($name);
    $options = array();

    if (!$this->isColumnPrimaryKey($name) && $this->isColumnForeignKey($name))
    {
      $options[] = sprintf('\'model\' => \'%s\', \'add_empty\' => %s', $this->getForeignTable($name)->getOption('name'), isset($column['notnull']) ? 'false' : 'true');
    }

    return count($options) ? sprintf('array(%s)', implode(', ', $options)) : '';
  }

  /**
   * Returns a sfValidator class name for a given column.
   *
   * @param  ColumnMap A ColumnMap object
   *
   * @return string    The name of a subclass of sfValidator
   */
  public function getValidatorClassForColumn($columnName)
  {
    $column = $this->table->getDefinitionOf($columnName);
    switch ($column['type'])
    {
      case 'boolean':
        $validatorSubclass = 'Boolean';
        break;
      case 'string':
      case 'clob':
      case 'glob':
        $validatorSubclass = 'String';
        break;
      case 'float':
      case 'decimal':
      case 'integer':
        $validatorSubclass = 'Number';
        break;
      case 'integer':
        $validatorSubclass = 'Integer';
        break;
      case 'date':
        $validatorSubclass = 'Date';
        break;
      case 'time':
        $validatorSubclass = 'Time';
        break;
      case 'timestamp':
        $validatorSubclass = 'DateTime';
        break;
      default:
        $validatorSubclass = 'Pass';
    }

    if ($this->isColumnPrimaryKey($columnName) || $this->isColumnForeignKey($columnName))
    {
      $validatorSubclass = 'DoctrineChoice';
    }

    return sprintf('sfValidator%s', $validatorSubclass);
  }

  /**
   * Returns a PHP string representing options to pass to a validator for a given column.
   *
   * @param  ColumnMap A ColumnMap object
   *
   * @return string    The options to pass to the validator as a PHP string
   */
  public function getValidatorOptionsForColumn($name)
  {
    $column = $this->table->getDefinitionOf($name);
    $options = array();

    if ($this->isColumnForeignKey($name))
    {
      $options[] = sprintf('\'model\' => \'%s\'', $this->getForeignTable($name)->getOption('name'));
    }
    else if ($this->isColumnPrimaryKey($name))
    {
      $options[] = sprintf('\'model\' => \'%s\', \'column\' => \'%s\'', $this->modelName, $name);
    }
    else
    {
      switch ($column['type'])
      {
        case 'string':
          if ($column['length'])
          {
            $options[] = sprintf('\'max_length\' => %s', $column['length']);
          }
          break;
      }
    }

    if (!$this->isColumnNotNull($name) || $this->isColumnPrimaryKey($name))
    {
      $options[] = '\'required\' => false';
    }

    return count($options) ? sprintf('array(%s)', implode(', ', $options)) : '';
  }

  /**
   * Returns the maximum length for a column name.
   *
   * @return integer The length of the longer column name
   */
  public function getColumnNameMaxLength()
  {
    $max = 0;
    foreach ($this->table->getColumns() as $name => $column)
    {
      if (($m = strlen($name)) > $max)
      {
        $max = $m;
      }
    }

    foreach ($this->getManyToManyRelations() as $relation)
    {
      if (($m = strlen($this->underscore($relation['refTable']->getOption('name')).'_list')) > $max)
      {
        $max = $m;
      }
    }

    return $max;
  }

  /**
   * Returns an array of primary key column names.
   *
   * @return array An array of primary key column names
   */
  public function getPrimaryKeyColumNames()
  {
    return $this->table->getIdentifierColumnNames();
  }

  /**
   * Returns a PHP string representation for the array of all primary key column names.
   *
   * @return string A PHP string representation for the array of all primary key column names
   *
   * @see getPrimaryKeyColumNames()
   */
  public function getPrimaryKeyColumNamesAsString()
  {
    return sprintf('array(\'%s\')', implode('\', \'', $this->getPrimaryKeyColumNames()));
  }

  /**
   * Returns true if the current table is internationalized.
   *
   * @return Boolean true if the current table is internationalized, false otherwise
   */
  public function isI18n()
  {
    return $this->table->hasRelation('Translation');
  }

  /**
   * Returns the i18n model name for the current table.
   *
   * @return string The model class name
   */
  public function getI18nModel()
  {
    return $this->table->getRelation('Translation')->getTable()->create();
  }

  public function underscore($name)
  {
    return strtolower(preg_replace(array('/([A-Z]+)([A-Z][a-z])/', '/([a-z\d])([A-Z])/'), '\\1_\\2', $name));
  }

  /**
   * Loads all Doctrine builders.
   */
  protected function loadModels()
  {
    $models = Doctrine::loadModels($this->generatorManager->getConfiguration()->getModelDirs(),
                                    Doctrine::MODEL_LOADING_CONSERVATIVE);
    $models =  Doctrine::initializeModels($models);
    $this->models = Doctrine::filterInvalidModels($models);
    return $this->models;
  }
}