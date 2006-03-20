#!/usr/local/bin/php -q
<?php

/* Etk-Php Demo. Required Php 5.x */

define(FIFO, "/tmp/etk_server_fifo");
define(EtkTrue, 1);
define(EtkFalse, 0);

class Etk
{    
    static private $loop = true;
    static private $callbacks = Array();
    
    function Etk()
    { }
    
    function Connect()
    {
	exec("etk_server ".FIFO." > /dev/null &");
	sleep(1);
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
    
    function True()
    {
	return 1;
    }
    
    function False()
    {
	return 0;
    }
    
    function AddCallback($name, $callback)
    {
	if(!is_array(self::$callbacks[$name]))
	  self::$callbacks[$name] = Array();
	   
	array_push(self::$callbacks[$name], $callback);
    }
    
    function Main()
    {
	$this->loop = EtkTrue;
	
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
	$this->loop = EtkFalse;
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
    
    function Set($w)
    {
	$this->object = $w;
    }
    
    function Get()
    {
	return $this->object;
    }
    
    function ShowAll()
    {
	$this->Call("widget_show_all ".$this->Get());
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
    
    function PackStart($child, $fill = EtkTrue, $expand = EtkTrue, $padding = 0)
    {
	$this->Call("box_pack_start ". $this->Get(). " " .$child->Get(). " $fill $expand $padding");
    }
}

class VBox Extends Box
{
    function VBox($homogenous = EtkFalse, $padding = 0)
    {
	parent::Box();
	$this->Set($this->Call("vbox_new $homogenous $padding"));
    }
}

class HBox Extends Box
{
    function HBox($homogenous = EtkFalse, $padding = 0)
    {
	parent::Box();
	$this->Set($this->Call("hbox_new $homogenous $padding"));
    }
}

/* Main Application */

$etk = new Etk();
$etk->Connect();
$etk->Call("init");

$win = new Window("Etk-Php Demo");
$win->SignalConnect("delete_event", _window_deleted_cb);

$vbox = new VBox(EtkFalse, 0);

$button1 = new Button("Php owns!");
$button1->SignalConnect("clicked", _button_1_clicked_cb);

$button2 = new Button("Etk owns!");
$button2->SignalConnect("clicked", _button_2_clicked_cb);

$check_button1 = new CheckButton("Check me!");

$entry = new Entry("Moo!");

$button3 = new Button("Get Text");
$button3->SignalConnect("clicked", _button_3_clicked_cb);

$vbox->PackStart($button1);
$vbox->PackStart($button2);
$vbox->PackStart($check_button1);
$vbox->PackStart($entry);
$vbox->PackStart($button3);

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
		
function _button_3_clicked_cb()
{
    global $entry;
    
    printf("Text = ".$entry->TextGet()."\n");
}
		
function _window_deleted_cb()
{
    global $etk;
    
    $etk->MainQuit();
}

?>
