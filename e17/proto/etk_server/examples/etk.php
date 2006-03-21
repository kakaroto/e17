#!/usr/local/bin/php -q
<?php

/* Etk-Php Demo. Requires Php 5.x */

define(FIFO, "/tmp/etk_server_fifo");

class Etk
{
    static private $loop = true;
    static private $callbacks = Array();
    const True = 1;
    const False = 0;

    const StockSizeSmall = 0;
    const StockSizeMedium = 1;
    const StockSizeBig = 2;

    const StockNO_STOCK = 0;
    const StockADDRESS_BOOK_NEW = 1;
    const StockAPPOINTMENT_NEW = 2;
    const StockBOOKMARK_NEW = 3;
    const StockCONTACT_NEW = 4;
    const StockDialogOk = 5;
    const StockDialogCancel = 6;
    const StockDIALOG_YES = 7;
    const StockDIALOG_NO = 8;
    const StockDIALOG_CLOSE = 9;
    const StockDOCUMENT_NEW = 10;
    const StockDOCUMENT_OPEN = 11;
    const StockDOCUMENT_PRINT = 12;
    const StockDOCUMENT_PRINT_PREVIEW = 13;
    const StockDOCUMENT_PROPERTIES = 14;
    const StockDOCUMENT_SAVE_AS = 15;
    const StockDOCUMENT_SAVE = 16;
    const StockEDIT_CLEAR = 17;
    const StockEDIT_COPY = 18;
    const StockEDIT_CUT = 19;
    const StockEDIT_FIND = 20;
    const StockEDIT_PASTE = 21;
    const StockEDIT_REDO = 22;
    const StockEDIT_UNDO = 23;
    const StockEDIT_DELETE = 24;
    const StockEDIT_FIND_REPLACE = 25;
    const StockEDIT_SELECT_ALL = 26;
    const StockFOLDER_NEW = 27;
    const StockFORMAT_INDENT_LESS = 28;
    const StockFORMAT_INDENT_MORE = 29;
    const StockFORMAT_JUSTIFY_CENTER = 30;
    const StockFORMAT_JUSTIFY_FILL = 31;
    const StockFORMAT_JUSTIFY_LEFT = 32;
    const StockFORMAT_JUSTIFY_RIGHT = 33;
    const StockFORMAT_TEXT_BOLD = 34;
    const StockFORMAT_TEXT_ITALIC = 35;
    const StockFORMAT_TEXT_STRIKETHROUGH = 36;
    const StockFORMAT_TEXT_UNDERLINE = 37;
    const StockGO_BOTTOM = 38;
    const StockGO_DOWN = 39;
    const StockGO_FIRST = 40;
    const StockGO_HOME = 41;
    const StockGO_JUMP = 42;
    const StockGO_LAST = 43;
    const StockGO_NEXT = 44;
    const StockGO_PREVIOUS = 45;
    const StockGO_TOP = 46;
    const StockGO_UP = 47;
    const StockLIST_ADD = 48;
    const StockLIST_REMOVE = 49;
    const StockMAIL_MESSAGE_NEW = 50;
    const StockMAIL_FORWARD = 51;
    const StockMAIL_MARK_JUNK = 52;
    const StockMAIL_REPLY_ALL = 53;
    const StockMAIL_REPLY_SENDER = 54;
    const StockMAIL_SEND_RECEIVE = 55;
    const StockMEDIA_EJECT = 56;
    const StockMEDIA_PLAYBACK_PAUSE = 57;
    const StockMEDIA_PLAYBACK_START = 58;
    const StockMEDIA_PLAYBACK_STOP = 59;
    const StockMEDIA_RECORD = 60;
    const StockMEDIA_SEEK_BACKWARD = 61;
    const StockMEDIA_SEEK_FORWARD = 62;
    const StockMEDIA_SKIP_BACKWARD = 63;
    const StockMEDIA_SKIP_FORWARD = 64;
    const StockPROCESS_STOP = 65;
    const StockSYSTEM_LOCK_SCREEN = 66;
    const StockSYSTEM_LOG_OUT = 67;
    const StockSYSTEM_SEARCH = 68;
    const StockSYSTEM_SHUTDOWN = 69;
    const StockTAB_NEW = 70;
    const StockVIEW_REFRESH = 71;
    const StockWINDOW_NEW = 72;
    const StockACCESSORIES_CALCULATOR = 73;
    const StockACCESSORIES_CHARACTER_MAP = 74;
    const StockACCESSORIES_TEXT_EDITOR = 75;
    const StockHELP_BROWSER = 76;
    const StockINTERNET_GROUP_CHAT = 77;
    const StockINTERNET_MAIL = 78;
    const StockINTERNET_NEWS_READER = 79;
    const StockINTERNET_WEB_BROWSER = 80;
    const StockMULTIMEDIA_VOLUME_CONTROL = 81;
    const StockOFFICE_CALENDAR = 82;
    const StockPREFERENCES_DESKTOP_ACCESSIBILITY = 83;
    const StockPREFERENCES_DESKTOP_ASSISTIVE_TECHNOLOGY = 84;
    const StockPREFERENCES_DESKTOP_FONT = 85;
    const StockPREFERENCES_DESKTOP_KEYBOARD_SHORTCUTS = 86;
    const StockPREFERENCES_DESKTOP_LOCALE = 87;
    const StockPREFERENCES_DESKTOP_REMOTE_DESKTOP = 88;
    const StockPREFERENCES_DESKTOP_SOUND = 89;
    const StockPREFERENCES_DESKTOP_SCREENSAVER = 90;
    const StockPREFERENCES_DESKTOP_THEME = 91;
    const StockPREFERENCES_DESKTOP_WALLPAPER = 92;
    const StockPREFERENCES_SYSTEM_NETWORK_PROXY = 93;
    const StockPREFERENCES_SYSTEM_SESSION = 94;
    const StockPREFERENCES_SYSTEM_WINDOWS = 95;
    const StockSYSTEM_FILE_MANAGER = 96;
    const StockSYSTEM_INSTALLER = 97;
    const StockSYSTEM_SOFTWARE_UPDATE = 98;
    const StockSYSTEM_USERS = 99;
    const StockUTILITIES_SYSTEM_MONITOR = 100;
    const StockUTILITIES_TERMINAL = 101;
    const StockAPPLICATIONS_ACCESSORIES = 102;
    const StockAPPLICATIONS_DEVELOPMENT = 103;
    const StockAPPLICATIONS_GAMES = 104;
    const StockAPPLICATIONS_GRAPHICS = 105;
    const StockAPPLICATIONS_INTERNET = 106;
    const StockAPPLICATIONS_MULTIMEDIA = 107;
    const StockAPPLICATIONS_OFFICE = 108;
    const StockAPPLICATIONS_OTHER = 109;
    const StockAPPLICATIONS_SYSTEM = 110;
    const StockPREFERENCES_DESKTOP_PERIPHERALS = 111;
    const StockPREFERENCES_DESKTOP = 112;
    const StockPREFERENCES_SYSTEM = 113;
    const StockAUDIO_CARD = 114;
    const StockAUDIO_INPUT_MICROPHONE = 115;
    const StockBATTERY = 116;
    const StockCAMERA_PHOTO = 117;
    const StockCAMERA_VIDEO = 118;
    const StockCOMPUTER = 119;
    const StockDRIVE_CDROM = 120;
    const StockDRIVE_HARDDISK = 121;
    const StockDRIVE_REMOVABLE_MEDIA = 122;
    const StockINPUT_GAMING = 123;
    const StockINPUT_KEYBOARD = 124;
    const StockINPUT_MOUSE = 125;
    const StockMEDIA_CDROM = 126;
    const StockMEDIA_FLOPPY = 127;
    const StockMULTIMEDIA_PLAYER = 128;
    const StockNETWORK = 129;
    const StockNETWORK_WIRELESS = 130;
    const StockNETWORK_WIRED = 131;
    const StockPRINTER = 132;
    const StockPRINTER_REMOTE = 133;
    const StockVIDEO_DISPLAY = 134;
    const StockEMBLEM_FAVORITE = 135;
    const StockEMBLEM_IMPORTANT = 136;
    const StockEMBLEM_PHOTOS = 137;
    const StockEMBLEM_READONLY = 138;
    const StockEMBLEM_SYMBOLIC_LINK = 139;
    const StockEMBLEM_SYSTEM = 140;
    const StockEMBLEM_UNREADABLE = 141;
    const StockFACE_ANGEL = 142;
    const StockFACE_CRYING = 143;
    const StockFACE_DEVIL_GRIN = 144;
    const StockFACE_GLASSES = 145;
    const StockFACE_GRIN = 146;
    const StockFACE_KISS = 147;
    const StockFACE_PLAIN = 148;
    const StockFACE_SAD = 149;
    const StockFACE_SMILE_BIG = 150;
    const StockFACE_SMILE = 151;
    const StockFACE_SURPRISE = 152;
    const StockFACE_WINK = 153;
    const StockAPPLICATION_CERTIFICATE = 154;
    const StockAPPLICATION_X_EXECUTABLE = 155;
    const StockAUDIO_X_GENERIC = 156;
    const StockFONT_X_GENERIC = 157;
    const StockIMAGE_X_GENERIC = 158;
    const StockPACKAGE_X_GENERIC = 159;
    const StockTEXT_HTML = 160;
    const StockTEXT_X_GENERIC = 161;
    const StockTEXT_X_GENERIC_TEMPLATE = 162;
    const StockTEXT_X_SCRIPT = 163;
    const StockVIDEO_X_GENERIC = 164;
    const StockX_DIRECTORY_DESKTOP = 165;
    const StockX_DIRECTORY_NORMAL_DRAG_ACCEPT = 166;
    const StockX_DIRECTORY_NORMAL_HOME = 167;
    const StockX_DIRECTORY_NORMAL_OPEN = 168;
    const StockX_DIRECTORY_NORMAL = 169;
    const StockX_DIRECTORY_NORMAL_VISITING = 170;
    const StockX_DIRECTORY_REMOTE = 171;
    const StockX_DIRECTORY_REMOTE_SERVER = 172;
    const StockX_DIRECTORY_REMOTE_WORKGROUP = 173;
    const StockX_DIRECTORY_TRASH_FULL = 174;
    const StockX_DIRECTORY_TRASH = 175;
    const StockX_OFFICE_ADDRESS_BOOK = 176;
    const StockX_OFFICE_CALENDAR = 177;
    const StockX_OFFICE_DOCUMENT = 178;
    const StockX_OFFICE_PRESENTATION = 179;
    const StockX_OFFICE_SPREADSHEET = 180;
    const StockPLACES_FOLDER = 181;
    const StockPLACES_FOLDER_REMOTE = 182;
    const StockPLACES_FOLDER_SAVED_SEARCH = 183;
    const StockPLACES_NETWORK_SERVER = 184;
    const StockPLACES_NETWORK_WORKGROUP = 185;
    const StockPLACES_START_HERE = 186;
    const StockPLACES_USER_DESKTOP = 187;
    const StockPLACES_USER_HOME = 188;
    const StockPLACES_USER_TRASH = 189;
    const StockAUDIO_VOLUME_HIGH = 190;
    const StockAUDIO_VOLUME_LOW = 191;
    const StockAUDIO_VOLUME_MEDIUM = 192;
    const StockAUDIO_VOLUME_MUTED = 193;
    const StockBATTERY_CAUTION = 194;
    const StockDIALOG_ERROR = 195;
    const StockDIALOG_INFORMATION = 196;
    const StockDIALOG_WARNING = 197;
    const StockDIALOG_QUESTION = 198;
    const StockFOLDER_DRAG_ACCEPT = 199;
    const StockFOLDER_OPEN = 200;
    const StockFOLDER_VISITING = 201;
    const StockIMAGE_LOADING = 202;
    const StockIMAGE_MISSING = 203;
    const StockMAIL_ATTACHMENT = 204;
    const StockNETWORK_ERROR = 205;
    const StockNETWORK_IDLE = 206;
    const StockNETWORK_OFFLINE = 207;
    const StockNETWORK_ONLINE = 208;
    const StockNETWORK_RECEIVE = 209;
    const StockNETWORK_TRANSMIT = 210;
    const StockNETWORK_TRANSMIT_RECEIVE = 211;
    const StockNETWORK_WIRELESS_ENCRYPTED = 212;
    const StockPRINTER_ERROR = 213;
    const StockUSER_TRASH_FULL = 214;

    const ResponseNone = -1;
    const ResponseReject = -2;
    const ResponseAccept = -3;
    const ResponseDeleteEvent = -4;
    const ResponseOk = -5;
    const ResponseCancel = -6;
    const ResponseClose = -7;
    const ResponseYes = -8;
    const ResponseNo = -9;
    const ResponseApply = -10;
    const ResponseHelp = -11;

    function __construct()
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

    function __construct()
    {
	parent::__construct();
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

    function __construct()
    {
	parent::__construct();
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
    function __construct($filename = "")
    {
	parent::__construct();

	if(!empty($filename))
	  $this->Set($this->Call("image_new_from_file \"$filename\""));
	else
	  $this->Set($this->Call("image_new"));
    }
}

class Container extends Widget
{
    function __construct()
    {
	parent::__construct();
    }

    function Add($widget)
    {
	$this->Call("container_add ".$this->Get(). " ".$widget->Get());
    }
}

abstract class Bin extends Container
{
    function __construct()
    {
	parent::__construct();
    }

    function ChildSet($child)
    {
	$this->Call("bin_child_set ".$this->Get()." ".$child->Get());
    }

    function ChildGet()
    {
    }
}

class Frame extends Bin
{
    function __construct($label)
    {
	parent::__construct();
	$this->Set($this->Call("frame_new \"$label\""));
    }

    function LabelSet($label)
    {
	$this->Call("frame_label_set ".$this->Get()." \"$label\"");
    }
}

class Button extends Bin
{
    var $image;

    function __construct($label = "", $img = "")
    {
	parent::__construct();

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
    function __construct($label = "")
    {
	parent::__construct($label);
	if(!empty($label))
	  $this->Set($this->Call("check_button_new_with_label \"$label\""));
	else
	  $this->Set($this->Call("check_button_new"));
    }
}

class Table extends Container
{
    function __construct($num_cols, $num_rows, $homogeneous = Etk::False)
    {
	parent::__construct();
	$this->Set($this->Call("table_new $num_cols $num_rows $homogeneous"));
    }

    function AttachDefaults(Widget $widget, $left_attach, $right_attach, $top_attach, $botton_attach)
    {
	$this->Call("table_attach_defaults ".$this->Get()." ".$widget->Get()." $left_attach $right_attach $top_attach $bottom_attach");
    }
}

class Window extends Container
{
    function __construct($title = "", $width = "", $height = "")
    {
	parent::__construct();

	$this->Set($this->Call("window_new"));
	$this->__construct_props($title, $width, $height);
    }

    protected function __construct2($title = "", $width = "", $height = "")
    {
	parent::__construct();
    }

    protected function __construct_props($title = "", $width = "", $height = "")
    {
	if(!empty($title))
	  $this->Call("window_title_set ".$this->Get(). " \"$title\"");

	if(!empty($width) && !empty($height))
	  $this->Call("window_resize $width $height");
    }
}

class Dialog extends Window
{
    function __construct($title = "", $width = "", $height = "")
    {
	parent::__construct2();
	$this->Set($this->Call("dialog_new"));
	$this->__construct_props($title, $width, $height);
    }

    function PackMainArea(Widget $widget, $expand = Etk::True, $fill = Etk::True, $padding = 0, $pack_at_end = Etk::False)
    {
	$this->Call("dialog_pack_in_main_area ".$this->Get()." ".$widget->Get()." $expand $fill $padding $pack_a_end");
    }

    function PackActionArea(Widget $widget, $expand = Etk::True, $fill = Etk::True, $padding = 0, $pack_at_end = Etk::False)
    {
	$this->Call("dialog_pack_in_action_area ".$this->Get()." ".$widget->Get()." $expand $fill $padding $pack_a_end");
    }

    function PackButtinActionArea(Button $button, $response_id, $expand = Etk::True, $fill = Etk::True, $padding = 0, $pack_at_end = Etk::False)
    {
	$this->Call("dialog_pack_button_in_action_area ".$this->Get()." ".$button->Get()." $response_id $expand $fill $padding $pack_at_end");
    }

    function ButtonAdd($label, $response_id)
    {
	$this->Call("dialog_button_add ".$this->Get()." \"$label\" $response_id");
    }

    function ButtonAddFromStock($stock_id, $response_id)
    {
	$this->Call("dialog_button_add_from_stock ".$this->Get()." $stock_id $response_id");
    }

    function HasSeperatorSet($has_seperator)
    {
	$this->Call("dialog_hash_seperator_set ".$this->Get()." $has_seperator");
    }
}

class Entry extends Widget
{
    function __construct($text = "")
    {
	parent::__construct();
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

class Label extends Widget
{
    function __construct($text = "")
    {
	parent::__construct();
	$this->Set($this->Call("label_new \"$text\""));
    }

    function TextSet($label)
    {
	$this->Call("label_set ".$this->Get()." \"$label\"");
    }
}

abstract class Box Extends Widget
{
    function __construct()
    {
	parent::__construct();
    }

    function PackStart($child, $fill = Etk::True, $expand = Etk::True, $padding = 0)
    {
	$this->Call("box_pack_start ". $this->Get(). " " .$child->Get(). " $fill $expand $padding");
    }
}

class VBox Extends Box
{
    function __construct($homogenous = Etk::False, $padding = 0)
    {
	parent::__construct();
	$this->Set($this->Call("vbox_new $homogenous $padding"));
    }
}

class HBox Extends Box
{
    function __construct($homogenous = Etk::False, $padding = 0)
    {
	parent::__construct();
	$this->Set($this->Call("hbox_new $homogenous $padding"));
    }
}

abstract class TreeModel
{
    var $model;
    var $etk;

    function __construct()
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
    function __construct($tree)
    {
	parent::__construct();
	$this->model = $this->etk->Call("tree_model_text_new ".$tree->Get());
    }
}

class TreeModelCheck extends TreeModel
{
    function __construct($tree)
    {
	parent::__construct();
	$this->model = $this->etk->Call("tree_model_checkbox_new ".$tree->Get());
    }
}

class TreeCol extends Object
{
    function __construct($tree, $title, $model, $width)
    {
	parent::__construct();
	$this->Set($this->Call("tree_col_new ".$tree->Get()." \"$title\" ".$model->Get()." $width"));
    }
}

class Tree extends Container
{
    const ModeList = 0;
    const ModeTree = 1;

    function __construct()
    {
	parent::__construct();
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
$clean_btn->SignalConnect("clicked", _clean_btn_clicked_cb);

$new_btn = new Button("New Task");
$new_btn->SignalConnect("clicked", _new_btn_clicked_cb);

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

function _new_btn_clicked_cb()
{
    $dialog = new Dialog("My Todo");
    $dialog->ButtonAddFromStock(Etk::StockDialogOk, Etk::ResponseOk);
    $dialog->ButtonAddFromStock(Etk::StockDialogCancel, Etk::ResponseCancel);

    $frame = new Frame("Add Task");
    $table = new Table(2, 3, Etk::False);
    $frame->Add($table);

    $label = new Label("Task:");
    $entry = new Entry();

    $table->AttachDefaults($label, 0, 0, 0, 0);
    $table->AttachDefaults($entry, 2, 2, 0, 0);

    $label = new Label("Priority:");
    $entry = new Entry();

    $table->AttachDefaults($label, 0, 0, 1, 1);
    $table->AttachDefaults($entry, 2, 2, 1, 1);

    $label = new Label("Deadline:");
    $entry = new Entry();

    $table->AttachDefaults($label, 0, 0, 2, 2);
    $table->AttachDefaults($entry, 2, 2, 2, 2);

    $dialog->PackMainArea($frame, Etk::False, Etk::False);
    $dialog->ShowAll();
}

function _clean_btn_clicked_cb()
{
    printf("Clean clicked!\n");
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
