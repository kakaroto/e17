#!/usr/local/bin/php -q
<?php

/* Etk-Php Demo. Required Php 5.x */

define(FIFO, "/tmp/etk_server_fifo");

class Etk
{    
    static private $loop = true;
    static private $callbacks = Array();
    const True = 1;
    const False = 0;
    
    function Etk()
    { }
    
    function Connect()
    {
	exec("etk_server ".FIFO." > /dev/null &");
	usleep(50000);
    }
    
    function Call($str)
    {	
	if(!$fd = fopen(FIFO, "w"))
	{
	    print("Cant open fifo: ". FIFO ."\n");
	    exit(-1);
	}

	if(fwrite($fd, "etk_".$str."\0") === FALSE)
	{
	    print("Cant write content to fifo!\n");
	    exit(-1);
	}

	fclose($fd);
	
	if(!$fd = fopen(FIFO, "r"))
	{
	    print("Cant open fifo: ". FIFO ."\n");
	    exit(-1);
	}

	if(!$ret = fread($fd, 4096))
	{
	    /*
	    print("Cant read contents from fifo!\n");
	    exit(-1);
	     */
	}

	fclose($fd);
	
	return $ret;
    }
        
    function AddCallback($name, $callback)
    {
	if(!is_array(self::$callbacks[$name]))
	  self::$callbacks[$name] = Array();
	   
	array_push(self::$callbacks[$name], $callback);
    }
    
    function Main()
    {
	$this->loop = Etk::True;
	
	while ( $this->loop )
	{
	    $event = Etk::Call("server_callback");
	    
	    if(!is_array(self::$callbacks[$event]))
	      continue;	   
	    
	    if(array_count_values(self::$callbacks[$event]) > 0)
	    {		
		foreach(self::$callbacks[$event] as $func)
		  $func();	    	   
	    }
	}	
    }
    
    function MainQuit()
    {
	$this->loop = Etk::False;
	$this->Call("main_quit");
	$this->Call("server_shutdown");
    }
}

class Object extends Etk
{
    var $object;
    
    function Object()
    {
	parent::Etk();
    }
    
    function Set($w)
    {
	$this->object = $w;
    }
    
    function Get()
    {
	return $this->object;
    }    
    
    function SignalConnect($name, $callback)
    {
	$this->Call("server_signal_connect \"$name\" ".$this->Get()." \"".$name."_".$this->object."\"");
	$this->AddCallback($name."_".$this->object, $callback);
    }    
}

class Widget extends Object
{
    var $widget;
    
    function Widget()
    {
	parent::Object();
    }
        
    function ShowAll()
    {
	$this->Call("widget_show_all ".$this->Get());
    }    
    
    function SizeRequestSet($width, $height)
    {
	$this->Call("widget_size_request_set ".$this->Get()." $width $height");
    }
}

class Image extends Widget
{    
    function Image($filename = "")
    {		
	parent::Widget();
	
	if(!empty($filename))	
	  $this->Set($this->Call("image_new_from_file \"$filename\""));
	else
	  $this->Set($this->Call("image_new"));
    }
}	    

class Container extends Widget
{
    function Container()
    {
	parent::Widget();
    }
    
    function Add($widget)
    {
	$this->Call("container_add ".$this->Get(). " ".$widget->Get());
    }	
}

class Bin extends Container
{
    function Bin()
    {
	parent::Container();
    }
    
    function ChildSet($child)
    {
	$this->Call("bin_child_set ".$this->Get()." ".$child->Get());
    }
    
    function ChildGet()
    {
    }
}
	
class Button extends Bin
{
    var $image;
    
    function Button($label = "", $img = "")
    {
	parent::Bin();
	
	if(!empty($label))
	  $this->Set($this->Call("button_new_with_label \"$label\""));
	else
	  $this->Set($this->Call("button_new"));
	
	if(!empty($img))
	{
	    $this->image = new Image($img);
	    $this->Call("button_image_set ".$this->Get()." ".$image->Get());
	}
    }
}

class CheckButton extends Button
{
    function CheckButton($label = "")
    {
	parent::Button($label);
	if(!empty($label))
	  $this->Set($this->Call("check_button_new_with_label \"$label\""));
	else
	  $this->Set($this->Call("check_button_new"));
    }
}

class Window extends Container
{
    function Window($title = "", $width = "", $height = "")
    {
	parent::Container();
	
	$this->Set($this->Call("window_new"));

	if(!empty($title))	
	  $this->Call("window_title_set ".$this->Get(). " \"$title\"");
	
	if(!empty($width) && !empty($height))
	  $this->Call("window_resize $width $height");	    
    }
}

class Entry extends Widget
{
    function Entry($text = "")
    {
	parent::Widget();
	$this->Set($this->Call("entry_new"));
	
	if(!empty($text))
	  $this->Call("entry_text_set ".$this->Get()." \"$text\"");
    }
    
    function TextGet()
    {
	$text = $this->Call("entry_text_get ".$this->Get());
	return $this->Call("server_var_get $text");
    }
}

class Box Extends Widget
{
    function Box()
    {
	parent::Widget();
    }
    
    function PackStart($child, $fill = Etk::True, $expand = Etk::True, $padding = 0)
    {
	$this->Call("box_pack_start ". $this->Get(). " " .$child->Get(). " $fill $expand $padding");
    }
}

class VBox Extends Box
{
    function VBox($homogenous = Etk::False, $padding = 0)
    {
	parent::Box();
	$this->Set($this->Call("vbox_new $homogenous $padding"));
    }
}

class HBox Extends Box
{
    function HBox($homogenous = Etk::False, $padding = 0)
    {
	parent::Box();
	$this->Set($this->Call("hbox_new $homogenous $padding"));
    }
}

class TreeModel
{
    var $model;
    var $etk;
    
    function TreeModel()
    {
	$this->etk = new Etk();
    }
    
    function Get()
    {
	return $this->model;
    }    
}

class TreeModelText extends TreeModel
{    
    function TreeModelText($tree)
    {
	parent::TreeModel();	
	$this->model = $this->etk->Call("tree_model_text_new ".$tree->Get());
    }    
}

class TreeModelCheck extends TreeModel
{    
    function TreeModelCheck($tree)
    {
	parent::TreeModel();
	$this->model = $this->etk->Call("tree_model_checkbox_new ".$tree->Get());
    }    
}

class TreeCol extends Object
{
    function TreeCol($tree, $title, $model, $width)
    {
	parent::Object();
	$this->Set($this->Call("tree_col_new ".$tree->Get()." \"$title\" ".$model->Get()." $width"));
    }    	
}

class Tree extends Container
{
    const ModeList = 0;
    const ModeTree = 1;
    
    function Tree()
    {
	parent::Container();
	$this->Set($this->Call("tree_new"));
    }
    
    function ModeSet($mode)
    {
	$this->Call("tree_mode_set $mode");
    }
    
    function Append()
    {
	$args = Array();
	$numargs = func_num_args();
	$arg_list = func_get_args();
	for ($i = 0; $i < $numargs; $i++)
	{
	    $arg = $arg_list[$i];

	    if(get_parent_class($arg) == "Object")
	      array_push($args, $arg->Get());
	    else
	    {
		if(is_string($arg))
		  array_push($args, "\"$arg\"");
		elseif($arg == NULL)
		  array_push($args, NULL);
		else
		  array_push($args, $arg);
	    }
	}

	return $this->Call("tree_append ".$this->Get()." ".implode(" ", $args));
    }

    function Build()
    {
	$this->Call("tree_build ".$this->Get());
    }    
}
    

/* Main Application */

$etk = new Etk();
$etk->Connect();
$etk->Call("init");

$win = new Window("My Todo");
$win->SignalConnect("delete_event", _window_deleted_cb);

$vbox = new VBox(Etk::False, 0);

$tree = new Tree();
$tree->SizeRequestSet(320, 200);
$tree->ModeSet(Tree::ModeList);
$col1 = new TreeCol($tree, "Done", new TreeModelCheck($tree), 20);
$col2 = new TreeCol($tree, "Task", new TreeModelText($tree), 170);
$col3 = new TreeCol($tree, "Priority", new TreeModelText($tree), 50);
$col4 = new TreeCol($tree, "Deadline", new TreeModelText($tree), 50);

$tree->Build();
$tree->Append($col1, Etk::False, $col2, "write an Etk-Php todo app", $col3, "High", $col4, date("d/m/y"), NULL);
$tree->Append($col1, Etk::True, $col2, "and make it work!", $col3, "High", $col4, date("d/m/y"), NULL);
$tree->Append($col1, Etk::False, $col2, "create some more classes", $col3, "Low", $col4, date("d/m/y"), NULL);
$tree->Append($col1, Etk::False, $col2, "bind this to sqlite", $col3, "Normal", $col4, date("d/m/y"), NULL);

$hbox = new HBox();

$clean_btn = new Button("Remove Done");

$new_btn = new Button("New Task");

$quit_btn = new Button("Quit");
$quit_btn->SignalConnect("clicked", _quit_btn_clicked_cb);

$hbox->PackStart($clean_btn, Etk::False, Etk::False);
$hbox->PackStart($new_btn, Etk::False, Etk::False);
$hbox->PackStart($quit_btn, Etk::False, Etk::False);

$vbox->PackStart($tree);
$vbox->PackStart($hbox, Etk::False, Etk::False);

$win->Add($vbox);

$win->ShowAll();

$etk->Main();

function _button_1_clicked_cb()
{
    printf("Button 1 clicked!\n");
}
		
function _button_2_clicked_cb()
{
    printf("Button 2 clicked!\n");
}
		
function _quit_btn_clicked_cb()
{
    global $etk;
    
    $etk->MainQuit();
}
		
function _window_deleted_cb()
{
    global $etk;
    
    $etk->MainQuit();
}

?>
