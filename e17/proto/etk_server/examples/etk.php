#!/usr/bin/php -q

<?php

define(FIFO, "/tmp/etk_server_fifo");
define(EtkTrue, 1);
define(EtkFalse, 0);

class Etk
{    
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
}

class Widget extends Etk
{
    var $widget;
    
    function Widget()
    {
	parent::Etk();
    }
    
    function Set($w)
    {
	$this->widget = $w;
    }
    
    function Get()
    {
	return $this->widget;
    }
    
    function ShowAll()
    {
	$this->Call("widget_show_all ".$this->Get());
    }
    
    function SignalConnect($name, $id)
    {
	$this->Call("server_signal_connect \"$name\" ".$this->Get()." \"$id\"");
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

class Button extends Widget
{
    var $image;
    
    function Button($label = "", $img = "")
    {
	parent::Widget();
	
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

Etk::Connect();
Etk::Call("init");

$win = new Window("Etk-Php Demo");
$win->SignalConnect("delete_event", "window_deleted");

$vbox = new VBox(EtkFalse, 0);

$button1 = new Button("Php owns!");
$button1->SignalConnect("clicked", "button_1_clicked");

$button2 = new Button("Etk owns!");
$button2->SignalConnect("clicked", "button_2_clicked");

$entry = new Entry("Moo!");

$button3 = new Button("Get Text");
$button3->SignalConnect("clicked", "button_3_clicked");

$vbox->PackStart($button1);
$vbox->PackStart($button2);
$vbox->PackStart($entry);
$vbox->PackStart($button3);

$win->Add($vbox);

$win->ShowAll();

$loop = EtkTrue;
while ( $loop )
{
    $event = Etk::Call("server_callback");
    
    switch($event)
    {
     case "button_1_clicked":
	print("Button 1 clicked!\n");
	break;
	
     case "button_2_clicked":
	print("Button 2 clicked!\n");
	break;

     case "button_3_clicked":
	printf("Text = ".$entry->TextGet());
	break;
	
     case "window_deleted":
	$loop = EtkFalse;
	break;
    }
}

Etk::Call("main_quit");
Etk::Call("server_shutdown");

?>
