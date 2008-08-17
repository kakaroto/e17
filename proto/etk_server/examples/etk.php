<?php

/* Etk-Php Demo. Requires Php 5.x */

define(FIFO, "/tmp/etk_server_fifo");

class Etk
{
    static $socket = "";
    static $result;
    static private $app_id;
    static private $var_id;
    static private $loop = true;
    static private $callbacks = Array();
    const True = 1;
    const False = 0;

    const StockSizeSmall = 0;
    const StockSizeMedium = 1;
    const StockSizeBig = 2;

    const StockNoStock = 0;
    const StockAddressBookNew = 1;
    const StockAppointmentNew = 2;
    const StockBookmarkNew = 3;
    const StockContactNew = 4;
    const StockDialogOk = 5;
    const StockDialogCancel = 6;
    const StockDialogYes = 7;
    const StockDialogNo = 8;
    const StockDialogClose = 9;
    const StockDocumentNew = 10;
    const StockDocumentOpen = 11;
    const StockDocumentPrint = 12;
    const StockDocumentPrintPreview = 13;
    const StockDocumentProperties = 14;
    const StockDocumentSaveAs = 15;
    const StockDocumentSave = 16;
    const StockEditClear = 17;
    const StockEditCopy = 18;
    const StockEditCut = 19;
    const StockEditFind = 20;
    const StockEditPaste = 21;
    const StockEditRedo = 22;
    const StockEditUndo = 23;
    const StockEditDelete = 24;
    const StockEditFindReplace = 25;
    const StockEditSelectAll = 26;
    const StockFolderNew = 27;
    const StockFormatIndentLess = 28;
    const StockFormatIndentMore = 29;
    const StockFormatJustifyCenter = 30;
    const StockFormatJustifyFill = 31;
    const StockFormatJustifyLeft = 32;
    const StockFormatJustifyRight = 33;
    const StockFormatTextBold = 34;
    const StockFormatTextItalic = 35;
    const StockFormatTextStrikethrough = 36;
    const StockFormatTextUnderline = 37;
    const StockGoBottom = 38;
    const StockGoDown = 39;
    const StockGoFirst = 40;
    const StockGoHome = 41;
    const StockGoJump = 42;
    const StockGoLast = 43;
    const StockGoNext = 44;
    const StockGoPrevious = 45;
    const StockGoTop = 46;
    const StockGoUp = 47;
    const StockListAdd = 48;
    const StockListRemove = 49;
    const StockMailMessageNew = 50;
    const StockMailForward = 51;
    const StockMailMarkJunk = 52;
    const StockMailReplyAll = 53;
    const StockMailReplySender = 54;
    const StockMailSendReceive = 55;
    const StockMediaEject = 56;
    const StockMediaPlaybackPause = 57;
    const StockMediaPlaybackStart = 58;
    const StockMediaPlaybackStop = 59;
    const StockMediaRecord = 60;
    const StockMediaSeekBackward = 61;
    const StockMediaSeekForward = 62;
    const StockMediaSkipBackward = 63;
    const StockMediaSkipForward = 64;
    const StockProcessStop = 65;
    const StockSystemLockScreen = 66;
    const StockSystemLogOut = 67;
    const StockSystemSearch = 68;
    const StockSystemShutdown = 69;
    const StockTabNew = 70;
    const StockViewRefresh = 71;
    const StockWindowNew = 72;
    const StockAccessoriesCalculator = 73;
    const StockAccessoriesCharacterMap = 74;
    const StockAccessoriesTextEditor = 75;
    const StockHelpBrowser = 76;
    const StockInternetGroupChat = 77;
    const StockInternetMail = 78;
    const StockInternetNewsReader = 79;
    const StockInternetWebBrowser = 80;
    const StockMultimediaVolumeControl = 81;
    const StockOfficeCalendar = 82;
    const StockPreferencesDesktopAccessibility = 83;
    const StockPreferencesDesktopAssistiveTechnology = 84;
    const StockPreferencesDesktopFont = 85;
    const StockPreferencesDesktopKeyboardShortcuts = 86;
    const StockPreferencesDesktopLocale = 87;
    const StockPreferencesDesktopRemoteDesktop = 88;
    const StockPreferencesDesktopSound = 89;
    const StockPreferencesDesktopScreensaver = 90;
    const StockPreferencesDesktopTheme = 91;
    const StockPreferencesDesktopWallpaper = 92;
    const StockPreferencesSystemNetworkProxy = 93;
    const StockPreferencesSystemSession = 94;
    const StockPreferencesSystemWindows = 95;
    const StockSystemFileManager = 96;
    const StockSystemInstaller = 97;
    const StockSystemSoftwareUpdate = 98;
    const StockSystemUsers = 99;
    const StockUtilitiesSystemMonitor = 100;
    const StockUtilitiesTerminal = 101;
    const StockApplicationsAccessories = 102;
    const StockApplicationsDevelopment = 103;
    const StockApplicationsGames = 104;
    const StockApplicationsGraphics = 105;
    const StockApplicationsInternet = 106;
    const StockApplicationsMultimedia = 107;
    const StockApplicationsOffice = 108;
    const StockApplicationsOther = 109;
    const StockApplicationsSystem = 110;
    const StockPreferencesDesktopPeripherals = 111;
    const StockPreferencesDesktop = 112;
    const StockPreferencesSystem = 113;
    const StockAudioCard = 114;
    const StockAudioInputMicrophone = 115;
    const StockBattery = 116;
    const StockCameraPhoto = 117;
    const StockCameraVideo = 118;
    const StockComputer = 119;
    const StockDriveCdrom = 120;
    const StockDriveHarddisk = 121;
    const StockDriveRemovableMedia = 122;
    const StockInputGaming = 123;
    const StockInputKeyboard = 124;
    const StockInputMouse = 125;
    const StockMediaCdrom = 126;
    const StockMediaFloppy = 127;
    const StockMultimediaPlayer = 128;
    const StockNetwork = 129;
    const StockNetworkWireless = 130;
    const StockNetworkWired = 131;
    const StockPrinter = 132;
    const StockPrinterRemote = 133;
    const StockVideoDisplay = 134;
    const StockEmblemFavorite = 135;
    const StockEmblemImportant = 136;
    const StockEmblemPhotos = 137;
    const StockEmblemReadonly = 138;
    const StockEmblemSymbolicLink = 139;
    const StockEmblemSystem = 140;
    const StockEmblemUnreadable = 141;
    const StockFaceAngel = 142;
    const StockFaceCrying = 143;
    const StockFaceDevilGrin = 144;
    const StockFaceGlasses = 145;
    const StockFaceGrin = 146;
    const StockFaceKiss = 147;
    const StockFacePlain = 148;
    const StockFaceSad = 149;
    const StockFaceSmileBig = 150;
    const StockFaceSmile = 151;
    const StockFaceSurprise = 152;
    const StockFaceWink = 153;
    const StockApplicationCertificate = 154;
    const StockApplicationXExecutable = 155;
    const StockAudioXGeneric = 156;
    const StockFontXGeneric = 157;
    const StockImageXGeneric = 158;
    const StockPackageXGeneric = 159;
    const StockTextHtml = 160;
    const StockTextXGeneric = 161;
    const StockTextXGenericTemplate = 162;
    const StockTextXScript = 163;
    const StockVideoXGeneric = 164;
    const StockXDirectoryDesktop = 165;
    const StockXDirectoryNormalDragAccept = 166;
    const StockXDirectoryNormalHome = 167;
    const StockXDirectoryNormalOpen = 168;
    const StockXDirectoryNormal = 169;
    const StockXDirectoryNormalVisiting = 170;
    const StockXDirectoryRemote = 171;
    const StockXDirectoryRemoteServer = 172;
    const StockXDirectoryRemoteWorkgroup = 173;
    const StockXDirectoryTrashFull = 174;
    const StockXDirectoryTrash = 175;
    const StockXOfficeAddressBook = 176;
    const StockXOfficeCalendar = 177;
    const StockXOfficeDocument = 178;
    const StockXOfficePresentation = 179;
    const StockXOfficeSpreadsheet = 180;
    const StockPlacesFolder = 181;
    const StockPlacesFolderRemote = 182;
    const StockPlacesFolderSavedSearch = 183;
    const StockPlacesNetworkServer = 184;
    const StockPlacesNetworkWorkgroup = 185;
    const StockPlacesStartHere = 186;
    const StockPlacesUserDesktop = 187;
    const StockPlacesUserHome = 188;
    const StockPlacesUserTrash = 189;
    const StockAudioVolumeHigh = 190;
    const StockAudioVolumeLow = 191;
    const StockAudioVolumeMedium = 192;
    const StockAudioVolumeMuted = 193;
    const StockBatteryCaution = 194;
    const StockDialogError = 195;
    const StockDialogInformation = 196;
    const StockDialogWarning = 197;
    const StockDialogQuestion = 198;
    const StockFolderDragAccept = 199;
    const StockFolderOpen = 200;
    const StockFolderVisiting = 201;
    const StockImageLoading = 202;
    const StockImageMissing = 203;
    const StockMailAttachment = 204;
    const StockNetworkError = 205;
    const StockNetworkIdle = 206;
    const StockNetworkOffline = 207;
    const StockNetworkOnline = 208;
    const StockNetworkReceive = 209;
    const StockNetworkTransmit = 210;
    const StockNetworkTransmitReceive = 211;
    const StockNetworkWirelessEncrypted = 212;
    const StockPrinterError = 213;
    const StockUserTrashFull = 214;
    const StockNumStockIds = 215;

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
    {
	static $_socket;
	static $_var_id = 0;
	static $_app_id = "";
	
	$this->socket = &$_socket;
	$this->var_id = &$_var_id;
	$this->app_id = &$_app_id;
    }   
    
    function Init()
    {
	if($this->app_id == "")
	{
	    $this->app_id = $this->Call(1, "server_init");
	}
    }
    
    function Connect()
    {
	exec("etk_server > /dev/null &");
	usleep(50000);
	
	$this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	$this->result = socket_connect($this->socket, "127.0.0.1", "8080");
    }

    function Call($mode, $str)
    {
	if($this->socket == "")
	{
	    print("NO SOCKET!\n");
	    return;
	}

	if($mode == 0)
	{
	    $m = "a";
	}
	else
	{
	    $m = "s";
	}		
	
	$tvar_id = "_".$this->var_id;
	$this->var_id++;
	
	$in = "$m ".$this->app_id.$tvar_id." etk_".$str."\0";
	if(socket_write($this->socket, $in, strlen($in)) <= 0)
	{
	    print("Cant write content to socket!\n");
	    exit(-1);
	}
	
	if($mode == 0)
	  return $this->app_id.$tvar_id;

	$ret = socket_read($this->socket, 2048);
	if(!$ret)
	{
	     print("Cant read contents from socket!\n");
	     exit(-1);
	}

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
	$arr = Array();
	
	while ( $this->loop )
	{
	    $event = $this->Call(1, "server_callback");
	    
	    if(strstr($event, " "))
	    {			
		$pattern = '/(num|str)="(.*?)(?<!\\\)"/';
		$tmp = explode(" ", $event, 2);
		$event = $tmp[0];
		preg_match_all($pattern, $tmp[1], $arr);
	    }
	    
	    if(!is_array(self::$callbacks[$event]))
	      continue;

	    if(array_count_values(self::$callbacks[$event]) > 0)
	    {
		foreach(self::$callbacks[$event] as $func)
		  $func($arr);
	    }
	}
    }

    function MainQuit()
    {
	$this->loop = Etk::False;
	$this->Call(0, "main_quit");
	$this->Call(0, "server_shutdown");
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
	$this->Call(0, "server_signal_connect \"$name\" ".$this->Get()." \"".$name."_".$this->object."\"");
	$this->AddCallback($name."_".$this->object, $callback);
    }
    
    function Destroy()
    {
	$this->Call(0, "object_destroy ".$this->Get());
	$this->Set("");	
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
	$this->Call(0, "widget_show_all ".$this->Get());
    }

    function SizeRequestSet($width, $height)
    {
	$this->Call(0, "widget_size_request_set ".$this->Get()." $width $height");
    }
}

class Image extends Widget
{
    function __construct($filename = "")
    {
	parent::__construct();

	if(!empty($filename))
	  $this->Set($this->Call(0, "image_new_from_file \"$filename\""));
	else
	  $this->Set($this->Call(0, "image_new"));
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
	$this->Call(0, "container_add ".$this->Get(). " ".$widget->Get());
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
	$this->Call(0, "bin_child_set ".$this->Get()." ".$child->Get());
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
	$this->Set($this->Call(0, "frame_new \"$label\""));
    }

    function LabelSet($label)
    {
	$this->Call(0, "frame_label_set ".$this->Get()." \"$label\"");
    }
}

class Button extends Bin
{
    var $image;

    function __construct($label = "", $img = "")
    {
	parent::__construct();

	if(!empty($label))
	  $this->Set($this->Call(0, "button_new_with_label \"$label\""));
	else
	  $this->Set($this->Call(0, "button_new"));

	if(!empty($img))
	{
	    $this->image = new Image($img);
	    $this->Call(0, "button_image_set ".$this->Get()." ".$image->Get());
	}
    }
    
    function Pressed()
    {
	$this->Call(0, "button_pressed ".$this->Get());
    }
    
    function Released()
    {
	$this->Call(0, "button_released ".$this->Get());
    }
    
    function Clicked()
    {
	$this->Call(0, "button_clicked ".$this->Get());
    }
    
    function LabelSet($label)
    {
	$this->Call(0, "button_label_set ".$this->Get());
    }
    
    function LabelGet()
    {
	$label = $this->Call(1, "button_label_get ".$this->Get());
	return $this->Call(1, "server_var_get $label");    
    }
    
    function ImageSet($image)
    {
	$this->Call(0, "button_image_set ".$this->Get()." ".$image->Get());
    }
    
    function ImageGet()
    {
	return $this->Call(1, "button_image_get ".$this->Get());
    }
    
    function AlignmentSet($xalign, $yalign)
    {
	$this->Call(0, "button_alignment_set ".$this->Get()." $xalign $yalign");
    }
    
    function AlignmentGet(&$xalign, &$yalign)
    {
    }
}

class CheckButton extends Button
{
    function __construct($label = "")
    {
	parent::__construct($label);
	if(!empty($label))
	  $this->Set($this->Call(0, "check_button_new_with_label \"$label\""));
	else
	  $this->Set($this->Call(0, "check_button_new"));
    }
}

class Table extends Container
{
    function __construct($num_cols, $num_rows, $homogeneous = Etk::False)
    {
	parent::__construct();
	$this->Set($this->Call(0, "table_new $num_cols $num_rows $homogeneous"));
    }

    function AttachDefaults(Widget $widget, $left_attach, $right_attach, $top_attach, $bottom_attach)
    {
	$this->Call(0, "table_attach_defaults ".$this->Get()." ".$widget->Get()." $left_attach $right_attach $top_attach $bottom_attach");
    }
}

class Window extends Container
{
    function __construct($title = "", $width = "", $height = "")
    {
	parent::__construct();

	$this->Set($this->Call(0, "window_new"));
	$this->__construct_props($title, $width, $height);
    }

    protected function __construct2($title = "", $width = "", $height = "")
    {
	parent::__construct();
    }

    protected function __construct_props($title = "", $width = "", $height = "")
    {
	if(!empty($title))
	  $this->Call(0, "window_title_set ".$this->Get(). " \"$title\"");

	if(!empty($width) && !empty($height))
	  $this->Call(0, "window_resize $width $height");
    }
}

class Dialog extends Window
{
    function __construct($title = "", $width = "", $height = "")
    {
	parent::__construct2();
	$this->Set($this->Call(0, "dialog_new"));
	$this->__construct_props($title, $width, $height);
    }

    function PackMainArea(Widget $widget, $expand = Etk::True, $fill = Etk::True, $padding = 0, $pack_at_end = Etk::False)
    {
	$this->Call(0, "dialog_pack_in_main_area ".$this->Get()." ".$widget->Get()." $expand $fill $padding $pack_at_end");
    }

    function PackActionArea(Widget $widget, $expand = Etk::True, $fill = Etk::True, $padding = 0, $pack_at_end = Etk::False)
    {
	$this->Call(0, "dialog_pack_in_action_area ".$this->Get()." ".$widget->Get()." $expand $fill $padding $pack_at_end");
    }

    function PackButtinActionArea(Button $button, $response_id, $expand = Etk::True, $fill = Etk::True, $padding = 0, $pack_at_end = Etk::False)
    {
	$this->Call(0, "dialog_pack_button_in_action_area ".$this->Get()." ".$button->Get()." $response_id $expand $fill $padding $pack_at_end");
    }

    function ButtonAdd($label, $response_id)
    {
	$this->Call(0, "dialog_button_add ".$this->Get()." \"$label\" $response_id");
    }

    function ButtonAddFromStock($stock_id, $response_id)
    {
	$this->Call(0, "dialog_button_add_from_stock ".$this->Get()." $stock_id $response_id");
    }

    function HasSeperatorSet($has_seperator)
    {
	$this->Call(0, "dialog_hash_seperator_set ".$this->Get()." $has_seperator");
    }
}

class Entry extends Widget
{
    function __construct($text = "")
    {
	parent::__construct();
	$this->Set($this->Call(0, "entry_new"));

	if(!empty($text))
	  $this->Call(0, "entry_text_set ".$this->Get()." \"$text\"");
    }

    function TextGet()
    {
	$text = $this->Call(1, "entry_text_get ".$this->Get());
	return $this->Call(1, "server_var_get $text");
    }
}

class Label extends Widget
{
    function __construct($text = "")
    {
	parent::__construct();
	$this->Set($this->Call(0, "label_new \"$text\""));
    }

    function TextSet($label)
    {
	$this->Call(0, "label_set ".$this->Get()." \"$label\"");
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
	$this->Call(0, "box_pack_start ". $this->Get(). " " .$child->Get(). " $fill $expand $padding");
    }
}

class VBox Extends Box
{
    function __construct($homogenous = Etk::False, $padding = 0)
    {
	parent::__construct();
	$this->Set($this->Call(0, "vbox_new $homogenous $padding"));
    }
}

class HBox Extends Box
{
    function __construct($homogenous = Etk::False, $padding = 0)
    {
	parent::__construct();
	$this->Set($this->Call(0, "hbox_new $homogenous $padding"));
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
	$this->model = $this->etk->Call(0, "tree_model_text_new ".$tree->Get());
    }
}

class TreeModelCheck extends TreeModel
{
    function __construct($tree)
    {
	parent::__construct();
	$this->model = $this->etk->Call(0, "tree_model_checkbox_new ".$tree->Get());
    }
}

class TreeCol extends Object
{
    function __construct($tree, $title, $model, $width)
    {
	parent::__construct();
	$this->Set($this->Call(0, "tree_col_new ".$tree->Get()." \"$title\" ".$model->Get()." $width"));
    }
}

class Tree extends Container
{
    const ModeList = 0;
    const ModeTree = 1;

    function __construct()
    {
	parent::__construct();
	$this->Set($this->Call(0, "tree_new"));
    }

    function ModeSet($mode)
    {
	$this->Call(0, "tree_mode_set ".$this->Get()." $mode");
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

	return $this->Call(1, "tree_append ".$this->Get()." ".implode(" ", $args));
    }

    function Build()
    {
	$this->Call(0, "tree_build ".$this->Get());
    }
    
    function FirstRowGet()
    {
	return $this->Call(1, "tree_first_row_get ".$this->Get());
    }
    
    function NextRowGet($row)
    {
	return $this->Call(1, "tree_next_row_get ".$this->Get()." $row 0 0");
    }
    
    function PrevRowGet($row)
    {
	return $this->Call(1, "tree_prev_row_get ".$this->Get()." $row 0 0");
    }
    
    function LastRowGet()
    {
	return $this->Call(0, "tree_last_row_get ".$this->Get()." 0 0");
    }
}

/* Main Application */

$etk = new Etk();
$etk->Connect();
$etk->Init();

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
//$tree->Append($col1, Etk::False, $col2, "write an Etk-Php todo app", $col3, "High", $col4, date("d/m/y"), NULL);
//$tree->Append($col1, Etk::True, $col2, "and make it work!", $col3, "High", $col4, date("d/m/y"), NULL);
//$tree->Append($col1, Etk::False, $col2, "create some more classes", $col3, "Low", $col4, date("d/m/y"), NULL);
//$tree->Append($col1, Etk::False, $col2, "bind this to sqlite", $col3, "Normal", $col4, date("d/m/y"), NULL);
$lines = Array();
$lines = file("todo");
foreach($lines as $line)
{    
    
    if(strlen(trim($line)) == 0)
      continue;
    
    $items = explode('?||?||?', $line);
    $tree->Append($col1, $items[0], $col2, $items[1], $col3, $items[2], $col4, trim($items[3]), NULL);
}  

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
socket_close($etk->socket);
function _new_btn_clicked_cb()
{
    global $dialog;
    
    if(get_class($dialog["dia"]) == "Dialog")
      if($dialog["dia"]->Get() != "")
	return;
    
    $dialog["dia"] = new Dialog("My Todo");
    $dialog["dia"]->ButtonAddFromStock(Etk::StockDialogOk, Etk::ResponseOk);
    $dialog["dia"]->ButtonAddFromStock(Etk::StockDialogCancel, Etk::ResponseCancel);

    $dialog["fr"] = new Frame("Add Task");
    $dialog["tbl"] = new Table(2, 3, Etk::False);
    $dialog["fr"]->Add($dialog["tbl"]);

    $dialog["lb1"] = new Label("Task:");
    $dialog["en1"] = new Entry();

    $dialog["tbl"]->AttachDefaults($dialog["lb1"], 0, 0, 0, 0);
    $dialog["tbl"]->AttachDefaults($dialog["en1"], 2, 2, 0, 0);

    $dialog["lb2"] = new Label("Priority:");
    $dialog["en2"] = new Entry();

    $dialog["tbl"]->AttachDefaults($dialog["lb2"], 0, 0, 1, 1);
    $dialog["tbl"]->AttachDefaults($dialog["en2"], 2, 2, 1, 1);

    $dialog["lb3"] = new Label("Deadline:");
    $dialog["en3"] = new Entry();

    $dialog["tbl"]->AttachDefaults($dialog["lb3"], 0, 0, 2, 2);
    $dialog["tbl"]->AttachDefaults($dialog["en3"], 2, 2, 2, 2);

    $dialog["dia"]->PackMainArea($dialog["fr"], Etk::False, Etk::False);
    
    $dialog["dia"]->SignalConnect("response", _new_dialog_response_cb);
    $dialog["dia"]->ShowAll();
}

function _new_dialog_response_cb($args)
{
    global $dialog;
    global $tree;
    global $col1, $col2, $col3;
    
    $response = $args[2][0];
    if($response == Etk::ResponseOk)
    {
	$fd = fopen("todo", "a");
	if(!$fd)
	  return;
	
	$task = $dialog["en1"]->TextGet();
	$prio = $dialog["en2"]->TextGet();
	$date = $dialog["en3"]->TextGet();	
	
	fputs($fd, "0?||?||?$task?||?||?$prio?||?||?$date\n");	
	fclose($fd);
	
	$tree->Append($col1, Etk::False, $col2, $task, $col3, $prio, $col4, $date, NULL);	
    }

    $dialog["dia"]->Destroy();
}

function _clean_btn_clicked_cb()
{
    global $tree;
    global $col1, $col2, $col3;
    
    $row = $tree->FirstRowGet();
    $lrow = $tree->LastRowGet();
    
    //print("$row = $lrow\n");
    
    $i = 0;    

    print("row $i $row\n");
    
    $i++;
    while($i < 5)
    {
	$row = $tree->NextRowGet($row);
	print("row $i $row\n");	
	$i++;
    }
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
