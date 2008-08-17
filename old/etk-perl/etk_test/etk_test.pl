#!/usr/bin/perl -w
use strict;
use POSIX;
use Etk::Simple;
use Etk::Constants qw/:all/;
use Etk::Stock qw/:all/;

my $NUM_COLS = 2;
my $win = Etk::Window->new();
$win->TitleSet("Etk-Perl Test");
my $vbox_frames = Etk::VBox->new(0, 0);

$win->Add($vbox_frames);

# our frames, each frame is a category for examples
my %frames = (
    
    basic => { 
	label => "Basic Widgets", 
	widget => undef,
	table => undef,
	examples => 0
    },
    
    adv => { 
	label => "Advanced Widgets", 
	widget => undef,
	table => undef,
	examples => 0
    },
    
    cont => { 
	label => "Containers", 
	widget => undef,
	table => undef,
	examples => 0
    },
    
    dia => { 
	label => "Dialogs", 
	widget => undef,
	table => undef,
	examples => 0
    },
    
    misc => { 
	label => "Misc", 
	widget => undef,
	table => undef,
	examples => 0
    }
);

# buttons used to launch the examples
my %buttons = (
    
    button => {
	label => "Button", 
	frame => "basic", 
	cb => \&button_window_show
    },
    
    entry => {
	label => "Entry",
	frame => "basic", 
	cb => \&entry_window_show
    },    
    
    image => {
	label => "Image", 
	frame => "basic", 
	cb => \&image_window_show
    },
    
    slider => {
	label => "Slider", 
	frame => "basic", 
	cb => \&slider_window_show
    },
    
    progbar => {
	label => "Progress Bar",
	frame => "basic", 
	cb => \&progbar_window_show
    },
    
    canvas => {
	label => "Canvas",
	frame => "adv", 
	cb => \&canvas_window_show
    },
    
    tree => {
	label => "Tree",
	frame => "adv", 
	cb => \&tree_window_show
    },
    
    menu => {
	label => "Menu",
	frame => "adv", 
	cb => \&menu_window_show
    },
    
    combobox => {
	label => "Combobox",
	frame => "adv", 
	cb => \&combobox_window_show
    },
    
    iconbox => {
	label => "Iconbox",
	frame => "adv", 
	cb => \&iconbox_window_show
    },
    
    textview => {
	label => "Text View",
	frame => "adv", 
	cb => \&textview_window_show
    },
    
    table => {
	label => "Table",
	frame => "cont", 
	cb => \&table_window_show
    },
    
    paned => {
	label => "Paned",
	frame => "cont", 
	cb => \&paned_window_show
    },
    
    scrolledview => {
	label => "Scrolled View",
	frame => "cont", 
	cb => \&scrolledview_window_show
    },
    
    notebook => {
	label => "Notebook",
	frame => "cont", 
	cb => \&notebook_window_show
    },
    
    colorpicker => {
	label => "Colorpicker",
	frame => "dia",
	cb => \&colorpicker_window_show
    },
    
    filechooser => {
	label => "File Chooser",
	frame => "dia", 
	cb => \&filechooser_window_show
    },    
    
    dnd => {
	label => "X Drag / Drop",
	frame => "misc", 
	cb => \&dnd_window_show
    }
);

my $_iconbox_folder = "";
my %_iconbox_types = (
    jpg => "mimetypes/image-x-generic_48",
    jpeg => "mimetypes/image-x-generic_48",
    png => "mimetypes/image-x-generic_48",
    bmp => "mimetypes/image-x-generic_48",
    gif => "mimetypes/image-x-generic_48",
    mp3 => "mimetypes/audio-x-generic_48",
    ogg => "mimetypes/audio-x-generic_48",
    wav => "mimetypes/audio-x-generic_48",
    avi => "mimetypes/video-x-generic_48",
    mpg => "mimetypes/video-x-generic_48",
    mpeg => "mimetypes/video-x-generic_48",
    gz => "mimetypes/package-x-generic_48",
    tgz => "mimetypes/package-x-generic_48",
    bz2 => "mimetypes/package-x-generic_48",
    tbz2 => "mimetypes/package-x-generic_48",
    zip => "mimetypes/package-x-generic_48",
    rar => "mimetypes/package-x-generic_48"    
);

# Count number of examples per category
while (my ($key, $value) = each %buttons)
{
    $frames{ $value->{frame} }->{examples}++;
}

# Create frames and pack them
while (my ($key, $value) = each %frames)
{ 
    $value->{widget} = Etk::Frame->new($value->{label});
    $value->{table} =  Etk::Table->new($NUM_COLS, 
	($value->{examples} + $NUM_COLS - 1) / $NUM_COLS, 1);
    $vbox_frames->Append($value->{widget}, BoxStart);
    $value->{widget}->Add($value->{table});
    $value->{examples} = 0;
}

# Create buttons and attach them
while (my ($key, $value) = each %buttons)
{
    my $button = Etk::Button->new($value->{label});
    my $table = $frames{ $value->{frame} }->{table};
    my $j = $frames{ $value->{frame} }->{examples};
    $button->SignalConnect("clicked", $value->{cb});
    $table->AttachDefault($button, $j % $NUM_COLS, $j % $NUM_COLS, 
	floor($j / $NUM_COLS), floor($j / $NUM_COLS));
    $frames{ $value->{frame} }->{examples}++;
}

$win->SignalConnect("delete-event", \&main_window_delete);
$win->BorderWidthSet(5);
$win->ShowAll();

Etk::Main::run();

sub button_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Button Test");
    my $vbox = Etk::VBox->new(0, 0);
   
    my $button = Etk::Button->new("Normal Button");
    $vbox->Append($button);
    $button = Etk::Button->new("Button with an image");
    $button->ImageSet(Etk::Image->new("images/e_icon.png"));
    $vbox->Append($button);
    $button = Etk::Button->new();
    $vbox->Append($button);
    $button = Etk::CheckButton->new("Check Button");
    $vbox->Append($button);
    $button = Etk::CheckButton->new();
    $vbox->Append($button);

    my $radio = Etk::RadioButton->new("Radio Button");
    $vbox->Append($radio);

    my $radio2 = Etk::RadioButton->new($radio);
    $vbox->Append($radio2);

    my $toggle = Etk::ToggleButton->new("Toggle Button");
    $vbox->Append($toggle);
    $toggle = Etk::ToggleButton->new();
    $vbox->Append($toggle);

    $win->Add($vbox);
    $win->ShowAll();
}

sub image_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Image Test");
    my $table = Etk::Table->new(2, 2, 0);
    my $image1 = Etk::Image->new();
    $image1->SetFromFile("images/test.png");
    my $image2 = Etk::Image->new();
    $image2->SetFromFile("images/test.png");
    my $label1 = Etk::Label->new("Keep aspect");
    my $label2 = Etk::Label->new("Don't keep aspect");
    
    $image1->KeepAspectSet(1);
    $image2->KeepAspectSet(0);
    
    $table->AttachDefault($image1, 0, 0, 0, 0);
    $table->AttachDefault($image2, 1, 1, 0, 0);
    $table->Attach($label1, 0, 0, 1, 1, 2, 0, TableHExpand);
    $table->Attach($label2, 1, 1, 1, 1, 2, 0, TableHExpand);    
    
    $win->Add($table);
    $win->ShowAll();
}

sub entry_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Entry Test");
    my $vbox = Etk::VBox->new(0, 0);    
    $win->Add($vbox);

    my $frame = Etk::Frame->new("Normal Entry");
    $vbox->Append($frame, BoxStart, BoxExpandFill, 0);

    my $table = Etk::Table->new(2, 2, 0);
    $frame->Add($table);

    my $entry_normal = Etk::Entry->new();
    $entry_normal->TextSet("Here is some text");
    $table->Attach($entry_normal, 0, 0, 0, 0, 0, 0, TableHExpand | TableHFill);
    $entry_normal->SizeRequestSet(140, -1);

    my $image = Etk::Image->new(DocumentPrint, SizeSmall);
    $entry_normal->ImageSet(ImagePrimary, $image);
    $entry_normal->ClearButtonAdd();

    my $label_normal = Etk::Label->new("");
    $table->Attach($label_normal, 0, 1, 1, 1, 0, 0, TableHExpand | TableHFill);

    $image->SignalConnect("mouse-click", sub {
	    $label_normal->Set( $entry_normal->TextGet() )
    });

    $vbox->Append( Etk::HSeparator->new(), BoxStart, BoxNone, 6);

    $frame = Etk::Frame->new("Password Entry");
    $vbox->Append($frame, BoxStart, BoxExpandFill, 0);
    $vbox = Etk::VBox->new(0, 0);
    $frame->Add($vbox);

    my $password_entry = Etk::Entry->new();
    $password_entry->TextSet("Password");
    $password_entry->PasswordModeSet(1);
    $vbox->Append($password_entry, BoxStart, BoxFill, 0);

    my $button = Etk::CheckButton->new("Password Visible");
    $vbox->Append($button, BoxStart, BoxFill, 0);
    $button->SignalConnect("toggled", sub {
	    $password_entry->PasswordModeSet( ! $password_entry->PasswordModeGet() )
    });


    $win->ShowAll();
}

sub slider_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Slider Test");
    my $table = Etk::Table->new(2, 2, 0);
    my $slider1 = Etk::HSlider->new(0.0, 255.0, 128.0, 1.0, 10.0);
    my $slider2 = Etk::VSlider->new(0.0, 255.0, 128.0, 1.0, 10.0);
    my ($label1, $label2);
    
    $slider1->SizeRequestSet(130, 130);    
    $table->AttachDefault($slider1, 0, 0, 0, 0);
    $label1 = Etk::Label->new("128.00");
    $table->Attach($label1, 0, 0, 1, 1, 0, 0, FillNone);
    $slider1->SignalConnect("value-changed",
	sub {
	    my $self = shift;
	    my $value = shift;
	    $label1->Set( sprintf("%.2f", $value));
	}
    );
    
    $slider2->SizeRequestSet(130, 130);    
    $table->AttachDefault($slider2, 1, 1, 0, 0);       
    $label2 = Etk::Label->new("128.00");
    $table->Attach($label2, 1, 1, 1, 1, 0, 0, FillNone);
    $slider2->SignalConnect("value-changed",
	sub {
	    my $self = shift;
	    my $value = shift;
	    $label2->Set( sprintf("%.2f", $value));
	}
    );
    
    $win->Add($table);
    $win->BorderWidthSet(5);
    $win->ShowAll();    
}

sub progbar_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Progress Bar Test");
    my $vbox = Etk::VBox->new(1, 5);
    my ($pbar1, $pbar2);

    my $timer1 = Etk::Timer->new(0.05, 
	sub {
	    my $fraction = $pbar1->FractionGet();
	    $fraction += 0.01;
	    
	    $fraction = 0.0 if ($fraction > 1.0);
	    
	    $pbar1->TextSet(sprintf("%.0f%% done", $fraction * 100.0));
	    $pbar1->FractionSet($fraction);
	    
	    return 1;
	}
    );

    my $timer2 = Etk::Timer->new(0.025,
	sub {
	    $pbar2->Pulse();
	    return 1;
	}
    );

    $pbar1 = Etk::ProgressBar->new("0% done");
    $pbar1->SignalConnect("destroyed",
    	sub {
		$timer1->Delete();
	});
    $vbox->Append($pbar1);
    $pbar2 = Etk::ProgressBar->new("Loading...");
    $pbar2->PulseStepSet(0.015);
    $pbar2->SignalConnect("destroyed",
    	sub {
		$timer2->Delete();
	});
    $vbox->Append($pbar2);
    
    
    $win->Add($vbox);
    $win->ShowAll();    
}

sub canvas_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Canvas Test");
    $win->Add(Etk::Label->new("<b>Etk::Canvas is not implemented yet.</b>"));
    
    $win->BorderWidthSet(10);
    $win->ShowAll();
}

sub tree_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Tree Test");
    $win->Resize(440, 500);

    my $vbox = Etk::VBox->new(0, 0);
    $win->Add($vbox);
    
    my $tree = Etk::Tree->new();
    $tree->ModeSet(ModeTree);
    $tree->MultipleSelectSet(1);

    $tree->PaddingSet(5, 5, 5, 5);
    
    $vbox->Append($tree, BoxStart, BoxExpandFill, 0);
    
    my $col1 = $tree->ColNew("Column 1", 130, 0.0);
    my $mod1 = new Etk::Tree::Model::Image;
    $col1->ModelAdd($mod1);
    my $mod2 = new Etk::Tree::Model::Text;
    $col1->ModelAdd($mod2);

    my $col2 = $tree->ColNew("Column 2", 60, 1.0);
    $col2->ModelAdd(new Etk::Tree::Model::Double);

    my $col3 = $tree->ColNew("Column 3", 60, 0.0);
    $col3->ModelAdd(new Etk::Tree::Model::Image);

    my $col4 = $tree->ColNew("Column 4", 60, 0.5);
    $col4->ModelAdd(new Etk::Tree::Model::Checkbox);

    my $status = Etk::StatusBar->new();
    $vbox->Append($status, BoxStart, BoxFill, 0);


    $tree->SignalConnect("row-clicked",  sub {
	my $self = shift;
	my $row = shift;
	my $event = shift;
	my $mod = $row->ModelFieldsGet($mod2);
	my $msg = "Row \"$mod\" clicked (";
	$msg .= $event->{flags} & MouseTripleClick ? "Triple" :
		$event->{flags} & MouseDoubleClick ? "Double" : "Single";
	$msg .= ")";
	$status->MessagePush($msg, 0);

    });


    $col4->SignalConnect("cell-value-changed", 
	sub {
		my $self = shift;
		my $row = shift;

		my $mod = $row->ModelFieldsGet($mod2);
		my $msg = "Row \"$mod\" has been " . ($row->FieldsGet($self) ? "checked" : "unchecked");
		$status->MessagePush($msg, 0);
	}
    );

    $tree->SignalConnect("key-down", sub {
    	my $self = shift;
	my $event = shift;
	if ($event->{keyname} eq "Delete") {
		my $row;
		for ($row = $tree->FirstRowGet(); $row; $row = $row->WalkNext(1)) {
			if ($row->IsSelected()) {
				$row->Delete();
			}
		}
		return 0;
	}

    });

    $tree->Build();
    
    $tree->Freeze();

    for(my $i = 0; $i < 1000; $i++)
    {
	my $row = $tree->RowAppend();

	$row->ModelFieldsSet(0, $mod1,  Etk::Theme::IconPathGet(), 
		Etk::Stock::KeyGet(PlacesUserHome, SizeSmall));
	$row->ModelFieldsSet(0, $mod2, "Row " . (($i*3)+1));
	$row->FieldsSet(0, $col2, 10.0);
	$row->FieldsSet(0, $col3, "images/1star.png");
	$row->FieldsSet(0, $col4, 0);


	my $row2 = $tree->RowAppend($row);
	$row2->ModelFieldsSet(0, $mod1,  Etk::Theme::IconPathGet(), 
		Etk::Stock::KeyGet(PlacesUserHome, SizeSmall));
	$row2->ModelFieldsSet(0, $mod2, "Row " . (($i*3)+2));
	
	$row2->FieldsSet(0, $col2, 20.0);
	$row2->FieldsSet(0, $col3, "images/2stars.png");
	$row2->FieldsSet(0, $col4, 1);

	my $row3 = $tree->RowAppend($row2);
	$row3->ModelFieldsSet(0, $mod1,  Etk::Theme::IconPathGet(), 
		Etk::Stock::KeyGet(PlacesUserHome, SizeSmall));
	$row3->ModelFieldsSet(0, $mod2, "Row " . (($i*3)+3));
	$row3->FieldsSet(0, $col2, 30.0);
	$row3->FieldsSet(0, $col3, "images/3stars.png");
	$row3->FieldsSet(0, $col4, 1);

    }

    $tree->Thaw();

    $col1->SortSet(  sub {
	my ($col, $row1, $row2) = @_;
	my $a = $row1->ModelFieldsGet($mod2);
	my $b = $row2->ModelFieldsGet($mod2);
	return $a cmp $b;
    }, undef);
    
    $win->ShowAll();
}

sub menu_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Menu Test");
    $win->SizeRequestSet(325, 240);
    my $box = Etk::VBox->new(0, 0);

    my $menubar = Etk::Menu::Bar->new();

    $box->Append($menubar);

    my $toolbar = Etk::Toolbar->new();
    $box->Append($toolbar);

    $toolbar->Append(Etk::Button->new(EditCopy));
    $toolbar->Append(Etk::Button->new(EditCut));
    $toolbar->Append(Etk::Button->new(EditPaste));

    $toolbar->Append(Etk::VSeparator->new());

    $toolbar->Append(Etk::Button->new(EditUndo));
    $toolbar->Append(Etk::Button->new(EditRedo));

    $toolbar->Append(Etk::VSeparator->new());

    $toolbar->Append(Etk::Button->new(EditFind));

    my $label = Etk::Label->new("Click me :)");
    $label->AlignmentSet(0.5, 0.5);
    $label->PassMouseEventsSet(1);

    $box->Append($label, BoxStart, BoxExpandFill);

    my $statusbar = Etk::StatusBar->new();

    $box->Append($statusbar, BoxEnd);

    my $menu_item = _menu_test_item_new("File", $menubar, $statusbar);
    my $menu = Etk::Menu->new();
    $menu_item->SubmenuSet($menu);
    _menu_test_stock_item_new("Open", DocumentOpen, $menu, $statusbar);
    _menu_test_stock_item_new("Save", DocumentSave, $menu, $statusbar);
    
    $menu_item = _menu_test_item_new("Edit", $menubar, $statusbar);
    $menu = Etk::Menu->new();
    $menu_item->SubmenuSet($menu);
    _menu_test_stock_item_new("Cut", EditCut, $menu, $statusbar);
    _menu_test_stock_item_new("Copy", EditCopy, $menu, $statusbar);
    _menu_test_stock_item_new("Paste", EditPaste, $menu, $statusbar);
    
    $menu_item = _menu_test_item_new("Help", $menubar, $statusbar);
    $menu = Etk::Menu->new();
    $menu_item->SubmenuSet($menu);
    _menu_test_item_new("About", $menu, $statusbar);
   
    $menu = Etk::Menu->new();
    $win->SignalConnect("mouse-down", sub { $menu->Popup() });
    
    _menu_test_stock_item_new("Open", DocumentOpen, $menu, $statusbar);
    _menu_test_stock_item_new("Save", DocumentSave, $menu, $statusbar);
    _menu_seperator_new($menu);
    _menu_test_stock_item_new("Cut", EditCut, $menu, $statusbar);
    _menu_test_stock_item_new("Copy", EditCopy, $menu, $statusbar);
    _menu_test_stock_item_new("Paste", EditPaste, $menu, $statusbar);
    _menu_seperator_new($menu);
    $menu_item = _menu_test_item_new("Menu Item Test", $menu, $statusbar);

    # Sub menu 1

    my $submenu1 = Etk::Menu->new();
    $menu_item->SubmenuSet($submenu1);

    _menu_test_stock_item_new("Item with image", DocumentSave, 
	$submenu1, $statusbar);

    $menu_item = _menu_test_stock_item_new("Item with child", 
	DocumentOpen, $submenu1, $statusbar);
    _menu_seperator_new($submenu1);
    _menu_test_check_item_new("Item with check 1", $submenu1, $statusbar);
    _menu_test_check_item_new("Item with check 2", $submenu1, $statusbar);
    _menu_seperator_new($submenu1);
    
    my $radio = _menu_test_radio_item_new("Radio 1", undef, $submenu1, $statusbar);
    _menu_test_radio_item_new("Radio 2", $radio, $submenu1, $statusbar);
    _menu_test_radio_item_new("Radio 3", $radio, $submenu1, $statusbar);

    # Sub menu 2
    my $submenu2 = Etk::Menu->new();
    $menu_item->SubmenuSet($submenu2);
    _menu_test_item_new("Child Menu Test", $submenu2, $statusbar);

    $win->Add($box);
    $win->ShowAll();
}

sub _menu_test_item_new
{
    my ($label, $menubar, $statusbar) = @_;
    my $menu_item = Etk::Menu::Item->new($label);
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->MessagePush($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("unselected", 
    	sub { $statusbar->MessagePop(0) });
    
    return $menu_item;
}

sub _menu_test_stock_item_new
{
    my ($label, $stockid, $menubar, $statusbar) = @_;

    my $menu_item = Etk::Menu::Item::Image->new($label);
    my $image = Etk::Image->new($stockid, SizeSmall);
    $menu_item->Set($image);
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->MessagePush($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("unselected", 
    	sub { $statusbar->MessagePop(0) });

    return $menu_item;

}

sub _menu_test_check_item_new
{
    my ($label, $menubar, $statusbar) = @_;

    my $menu_item = Etk::Menu::Item::Check->new($label);
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->MessagePush($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("unselected", 
    	sub { $statusbar->MessagePop(0) });

    return $menu_item;
}

sub _menu_test_radio_item_new
{
    my ($label, $radio, $menubar, $statusbar) = @_;

    my $menu_item;
    if ($radio) 
    {
        $menu_item = Etk::Menu::Item::Radio->new($label, $radio);
    }
    else
    {
        $menu_item = Etk::Menu::Item::Radio->new($label);
    }
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->MessagePush($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("unselected", 
    	sub { $statusbar->MessagePop(0) });

    return $menu_item;
}

sub _menu_seperator_new
{
    my ($menubar) = @_;
    my $menu_item = Etk::Menu::Item::Separator->new();
    $menubar->Append($menu_item);
    return $menu_item;
}

sub __combobox_entry_populate 
{
    my $combo = shift;
    my $dir = shift;

    $combo->Clear();
    $combo->ItemPrepend(
    		Etk::Image::new_from_stock(PlacesFolder, SizeSmall),
		"..");

    while (<$dir/*>) {
	    if (-d) {
		$combo->ItemPrepend(
			Etk::Image::new_from_stock(PlacesFolder, SizeSmall),
			$_);
	    } else {
		$combo->ItemPrepend(
			Etk::Image::new_from_stock(TextXGeneric, SizeSmall),
			$_);
	    }

    }

}

sub combobox_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Combo Test");
    my $vbox = Etk::VBox->new(0, 3);
    
    my $frame = Etk::Frame->new("Simple combobox");
    $vbox->Append($frame);
    
    my $combobox = Etk::Combobox::new_default();
    $frame->Add($combobox);
    $combobox->ItemAppend("Test 1");
    $combobox->ItemAppend("Test 2");
    $combobox->ItemAppend("Test 3");    

    $frame = Etk::Frame->new("Entry combobox");
    $vbox->Append($frame);

    $combobox = Etk::Combobox::Entry->new();
    $combobox->SignalConnect("active-item-changed", sub {
	    print "TODO\n";
    });
    $combobox->ColumnAdd(EntryImage, 24, EntryNone, 0.0);
    $combobox->ColumnAdd(EntryLabel, 75, EntryExpand, 0.0);
    $combobox->Build();
    $frame->Add($combobox);
    __combobox_entry_populate($combobox, ".");

    $frame = Etk::Frame->new("Some stock icons");
    $vbox->Append($frame);
    
    my $vbox2 = Etk::VBox->new(0, 3);
    $frame->Add($vbox2);
    
    my $image = Etk::Image->new(DocumentNew, SizeBig);
    $vbox2->Append($image);
    
    $combobox = Etk::Combobox->new();
    $combobox->ColumnAdd(ColumnTypeImage, 24, 0, 0.0);
    $combobox->ColumnAdd(ColumnTypeLabel, 75, 1, 0.0);
    $combobox->Build();
    
    $vbox2->Append($combobox);
	
    for( my $i = DocumentNew; 
	$i <= FormatTextUnderline; $i++)
    {
	my $image2 = Etk::Image->new($i, SizeSmall);
	my $item = $combobox->ItemAppend($image2, Etk::Stock::label_get($i));
#	$item->ColSet(1, Etk::Stock::label_get($i));
	$item->DataSet($i);
    }

    $combobox->ActiveItemSet($combobox->ActiveItemGet());
    
    $combobox->SignalConnect("active-item-changed", 
	sub {
	    my $item = $combobox->ActiveItemGet();
	    my $stock_id = $item->DataGet();
	    $image->SetFromStock($stock_id, SizeBig);
	}
    );    
    
    $win->Add($vbox);
    $win->ShowAll();    
}

sub iconbox_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Iconbox Test");
    $win->SizeRequestSet(100, 100);
    $win->Resize(600, 330);
    
    $_iconbox_folder = "";    
    my $iconbox = Etk::Iconbox->new();
    my $model = Etk::Iconbox::Model->new($iconbox);
    $model->GeometrySet(150, 20, 2, 2);
    $model->IconGeometrySet(20, 0, 130, 16, 0.0, 0.5);
    _iconbox_folder_set($iconbox, "");
    
    $iconbox->SignalConnect("mouse-down", 
	sub {
	    my $self = shift;
	    my $event = shift;
	    return unless $event->{flags} & MouseDoubleClick;
	    my $icon = $iconbox->IconGetAtXy($event->{"canvas.x"},
		$event->{"canvas.y"}, 0, 1, 1);
	    return unless $icon;
	    if (-d $_iconbox_folder."/".$icon->LabelGet())
	    {
		_iconbox_folder_set($iconbox, $icon->LabelGet());
	    }
	}
    );    
        
    $win->Add($iconbox);
    $win->ShowAll();        
}

sub _iconbox_folder_set
{
    my $iconbox = shift;
    my $folder = shift;
    my $file = undef;
    
    $folder = $ENV{HOME} if($folder eq "");
    return if($folder eq "");      
    
    $iconbox->Clear();
    $iconbox->Append(Etk::Theme::IconPathGet(), "actions/go-up_48", "..");
    
    # Add directories
    opendir(DIR, $_iconbox_folder."/".$folder) or 
      die "can't opendir $folder: $!";
    while (defined($file = readdir(DIR))) {
	if (-d "$_iconbox_folder/$folder/$file" && $file  !~ /^\./)
	{
	    $iconbox->Append(Etk::Theme::IconPathGet(), 
		"places/folder_48", $file);
	}
    }
    closedir(DIR);
    
    # Add files
    opendir(DIR, $_iconbox_folder."/".$folder) or 
      die "can't opendir $folder: $!";
    while (defined($file = readdir(DIR))) {
	if (-f "$_iconbox_folder/$folder/$file" && $file  !~ /^\./)
	{
	    my @parts = split /\./, $file;
	    $parts[-1] =~ tr [A-Z] [a-z];
	    if($_iconbox_types{$parts[-1]})
	    {
		$iconbox->Append(Etk::Theme::IconPathGet(), 
		    $_iconbox_types{$parts[-1]}, $file);
	    }
	    else
	    {
		$iconbox->Append(Etk::Theme::IconPathGet(), 
		    "mimetypes/text-x-generic_48", $file);
	    }
	}
    }
    closedir(DIR);
    
    $_iconbox_folder .= "/".$folder;
}

sub textview_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Textview Test");
    my $vbox = Etk::VBox->new(1, 0);

    $win->Add($vbox);

    my $button = Etk::Button->new("Tag Presentation");
    $button->SignalConnect("clicked", sub {

	    my $win = Etk::Window->new();
	    $win->TitleSet("Etk-Perl Text View Test");
	    my $vbox = Etk::VBox->new(0, 0);
	    $win->Add($vbox);

	    $win->SizeRequestSet(150, 150);
	    $win->Resize(400, 300);

	    my $text_view = Etk::TextView->new();

	    my $text_block = $text_view->TextblockGet();

	    $text_block->TextSet(
	    join ('',
	      "<p align=\"center\"><b><u><font size=18>Etk Textblock</font></u></b></p> \n",
	      "<b><u><font size=12>Supported tags:</font></u></b>\n",
	      "<p left_margin=30>",
	         "<b>&lt;b&gt;:</b> <b>Bold</b>\n",
	         "<b>&lt;i&gt;:</b> <i>Italic</i>\n",
	         "<b>&lt;u&gt;:</b> <u>Underline</u>\n",
	      "</p>" ,
	      "<p left_margin=60>",
	            "<i>type:</i> Whether the text is single or double underlined\n",
	            "<i>color1:</i> The color of the first underline\n",
	            "<i>color2:</i> The color of the second underline\n",
	      "</p>"  ,
	      "<p left_margin=30>",
	         "<b>&lt;s&gt;:</b> <s>Strikethrough</s>\n",
	      "</p>"  ,
	      "<p left_margin=60>",
	            "<i>color:</i> The color of the strikethrough\n",
	      "</p>"  ,
	      "<p left_margin=30>",
	         "<b>&lt;font&gt;:</b>\n",
	      "</p>"  ,
	      "<p left_margin=60>",
	            "<i>face:</i> The face of the font\n",
	            "<i>size:</i> The size of the font\n",
	            "<i>color:</i> The color of the font\n",
	      "</p>"  ,
	      "<p left_margin=30>",
	         "<b>&lt;style&gt;:</b>\n",
	      "</p>"  ,
	      "<p left_margin=60>",
	            "<i>effect:</i> The type of effect to apply on the text\n",
	            "<i>color1:</i> The first color of the effect\n",
	            "<i>color2:</i> The second color of the effect\n",
	      "</p>") , 1);


	    $vbox->Append($text_view, BoxStart, BoxExpandFill);

	    $win->ShowAll();


    });
    $vbox->Append($button);

    $button = Etk::Button->new("Instant Messenger");
    $button->SignalConnect("clicked", sub {

	    my $win = Etk::Window->new();
	    $win->TitleSet("Etk Text View Test: Instant Messenger");
	    $win->Resize(300, 300);
	    $win->BorderWidthSet(3);
	    
	    my $vpaned = Etk::VPaned->new();
	    $win->Add($vpaned);

	    my $message_view = Etk::TextView->new();
	    $message_view->SizeRequestSet(200, 100);
	    $vpaned->Child1Set($message_view, 1);

	    my $tb = $message_view->TextblockGet();

	    $tb->TextSet(
	    	"<i>Connected with David P. and Chloe O.</i>\n\n".
		"<b><font color=#a82f2f>David P:</font></b> Hello Jack!\n".
		"<b><font color=#a82f2f>David P:</font></b> How are you my friend?\n", 1);

	    my $vbox = Etk::VBox->new(0, 0);
	    $vpaned->Child2Set($vbox, 0);

	    my $hbox = Etk::HBox->new(0, 0);
	    $vbox->Append($hbox, BoxStart, BoxNone, 0);

	    for my $b (FormatTextBold, FormatTextItalic, FormatTextUnderline) {
		    my $button = Etk::Button->new($b);
		    $hbox->Append($button);
	    }

	    my $editor_view = Etk::TextView->new();
	    $editor_view->SizeRequestSet(200, 80);
	    $vbox->Append($editor_view);
	    my @buddies = (
		"<b><font color=#16569e>Jack B:</font></b> ",
		"<b><font color=#609028>Chloe O:</font></b> ",
		"<b><font color=#a82f2f>David P:</font></b> "
	    );
	    my $num_messages = 0;

	    $editor_view->SignalConnect("key-down", sub {
		    my $self = shift->TextblockGet();
		    my $event = shift;
		    my $message_tb = shift->TextblockGet();
		    my $message = $self->TextGet(1);
		    if ($event->{key} eq "Return" || $event->{key} eq "KP_Enter"){
			    my $iter = Etk::TextBlock::Iter->new($message_tb);
			    $iter->ForwardEnd();
			    if ($message ne "") {
				    $message_tb->InsertMarkup($iter, $buddies[$num_messages % @buddies]);
				    $message_tb->InsertMarkup($iter, $message);
				    $message_tb->Insert($iter, "\n");

				    $self->Clear();
				    $num_messages++;
			    }
			    $iter->free();
			    return 0;
		    }
		    return 1;

	    }, $message_view);

	    $win->ShowAll();
    });
    $vbox->Append($button);
    
    $win->ShowAll();  

}

sub table_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Table Test");
    
    my @widgets;
    
    push @widgets, Etk::Button->new(DocumentOpen);
    $widgets[0]->LabelSet("Set Icon");
    
    push @widgets, 
      Etk::Label->new("App name"),
      Etk::Entry->new(),
      Etk::Label->new("Generic Info"),
      Etk::Entry->new(),
      Etk::Label->new("Comments"),
      Etk::Entry->new(),
      Etk::Label->new("Executable"),
      Etk::Entry->new(),
      Etk::Label->new("Window name"),
      Etk::Entry->new(),
      Etk::Label->new("Window class"),
      Etk::Entry->new(),
      Etk::Label->new("Startup notify"),
      Etk::CheckButton->new(),
      Etk::Label->new("Wait exit"),
      Etk::CheckButton->new();
    
    push @widgets, Etk::Button->new(DialogCancel);
    push @widgets, Etk::Button->new(DocumentSave);
    
    push @widgets, Etk::Image->new("images/test.png");

    push @widgets, Etk::Alignment->new(0.5, 0.5, 0, 0);

    $widgets[20]->Add($widgets[0]);

    my $vbox = Etk::VBox->new(0, 0);
    my $hbox = Etk::HBox->new(0, 0);
    my $table = Etk::Table->new(2, 10, 0);

    $vbox->Append($table);
    $vbox->Append($hbox, BoxEnd);
    $hbox->Append($widgets[18], BoxEnd);
    $hbox->Append($widgets[17], BoxEnd);

    $table->Attach($widgets[19], 0, 0, 0, 0, 0, 0, FillNone);
    $table->Attach($widgets[20], 1, 1, 0, 0, 0, 0, HExpand | HFill);

    my $index = 1;
    for my $i (2 .. 9) 
    {
        $table->Attach($widgets[$index], 0, 0, $i, $i, 0, 0, HFill);
        $table->AttachDefault($widgets[$index + 1], 1, 1, $i, $i);
        $index += 2;
    }
    $win->BorderWidthSet(5);
    $win->Add($vbox);
    $win->ShowAll();  
}

sub paned_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Paned Test");
    $win->SizeRequestSet(300,300);
    my $vbox = Etk::VBox->new(0,0);

    # Paned Area
    my $vpaned = Etk::VPaned->new();
    $vbox->Append($vpaned, BoxStart, BoxExpandFill);

    my $hpaned = Etk::HPaned->new();
    $vpaned->Child1Set($hpaned, 0);

    my $label = Etk::Label->new("HPaned Child 1");
    $label->AlignmentSet(0.5, 0.5);
    $hpaned->Child1Set($label, 1);

    $label = Etk::Label->new("HPaned Child 2");
    $label->AlignmentSet(0.5, 0.5);
    $hpaned->Child2Set($label, 0);

    $label = Etk::Label->new("VPaned Child 2");
    $label->AlignmentSet(0.5, 0.5);
    $vpaned->Child2Set($label, 1);

    my $hseparator = Etk::HSeparator->new();
    $vbox->Append($hseparator, BoxStart, BoxNone, 6);

    # Properties Area
    my $hbox = Etk::HBox->new(1, 0);
    $vbox->Append($hbox);

    my $frame = Etk::Frame->new("HPaned Properties");
    $hbox->Append($frame, BoxStart, BoxExpandFill);

    my $vbox2 = Etk::VBox->new(1, 0);
    $frame->Add($vbox2);
    
    my $check_button = Etk::CheckButton->new("Child 1 Expand");
    $check_button->ActiveSet(1);
    $vbox2->Append($check_button, BoxStart, BoxExpandFill);

    $check_button = Etk::CheckButton->new("Child 2 Expand");
    $vbox2->Append($check_button);

    $frame = Etk::Frame->new("VPaned Properties");
    $hbox->Append($frame, BoxStart, BoxExpandFill);

    $vbox2 = Etk::VBox->new(1, 0);
    $frame->Add($vbox2);
    $check_button = Etk::CheckButton->new("Child 1 Expand");
    $vbox2->Append($check_button, BoxStart, BoxExpandFill);
    $check_button = Etk::CheckButton->new("Child 2 Expand");
    $check_button->ActiveSet(1);
    $vbox2->Append($check_button, BoxStart, BoxExpandFill);

    $win->Add($vbox);
    $win->ShowAll();
   
}

sub scrolledview_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Scrolled View Test");
    $win->SizeRequestSet(180,180);

    my $scrolledview = Etk::ScrolledView->new();
    my $button = Etk::Button->new("Scrolled View Test");
    $button->SizeRequestSet(300, 300);
    $scrolledview->AddWithViewport($button);

    $win->Add($scrolledview);
    $win->ShowAll();
}

sub notebook_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Notebook Test");

    my $vbox = Etk::VBox->new(0, 0);

    my $notebook = Etk::Notebook->new();
    $vbox->Append($notebook, BoxStart, BoxExpandFill);

    my @widgets;
    
    push @widgets, Etk::Button->new(DocumentOpen);
    $widgets[0]->LabelSet("Set Icon");
    
    push @widgets, 
      Etk::Label->new("App name"), Etk::Entry->new(),
      Etk::Label->new("Generic Info"), Etk::Entry->new(),
      Etk::Label->new("Comments"), Etk::Entry->new(),
      Etk::Label->new("Executable"), Etk::Entry->new(),
      Etk::Label->new("Window name"), Etk::Entry->new(),
      Etk::Label->new("Window class"), Etk::Entry->new(),
      Etk::Label->new("Startup notify"), Etk::CheckButton->new(),
      Etk::Label->new("Wait exit"), Etk::CheckButton->new();
    
    push @widgets, Etk::Image->new("images/test.png");
    push @widgets, Etk::Alignment->new(0.5, 0.5, 0, 0);
    $widgets[18]->Add($widgets[0]);
    my $table = Etk::Table->new(2, 10, 0);
    $table->Attach($widgets[17], 0, 0, 0, 0, 0, 0, FillNone);
    $table->Attach($widgets[18], 1, 1, 0, 0, 0, 0, HExpand | HFill);

    my $index = 1;
    for my $i (2 .. 9) 
    {
        $table->Attach($widgets[$index], 0, 0, $i, $i, 0, 0, HFill);
        $table->AttachDefault($widgets[$index + 1], 1, 1, $i, $i);
        $index += 2;
    }

    $notebook->PageAppend("Tab 1 - Table test", $table);

    my $alignment = Etk::Alignment->new(0.5, 0.5, 0.2, 0);
    my $vbox2 = Etk::VBox->new(0, 3);
    $alignment->Add($vbox2);
 
    my $button = Etk::Button->new("Normal Button");
    $vbox2->Append($button);
    
    $button = Etk::ToggleButton->new("Toggle Button");
    $vbox2->Append($button);
    
    $button = Etk::CheckButton->new("Check Button");
    $vbox2->Append($button);
    
    $button = Etk::CheckButton->new();
    $vbox2->Append($button);
    
    $button = Etk::RadioButton->new("Radio button");
    $vbox2->Append($button);

    my $button2 = Etk::RadioButton->new($button);
    $vbox2->Append($button2);
    
    $notebook->PageAppend("Tab 2 - Button test", $alignment);

    $vbox->Append(Etk::HSeparator->new(), 0, 0, 5);

    $alignment = Etk::Alignment->new(0.5, 0.5, 0, 1);
    $vbox->Append($alignment, 0, 0, 0);
    my $hbox = Etk::HBox->new(1, 0);
    $alignment->Add($hbox);

    $button = Etk::Button->new(GoPrevious);
    $button->LabelSet("Previous");
    $button->SignalConnect("clicked", sub {
		$notebook->PagePrev();
    });
    $hbox->Append($button);
    
    $button = Etk::Button->new(GoNext);
    $button->LabelSet("Next");
    $button->SignalConnect("clicked", sub {
		$notebook->PageNext();
    });
    $hbox->Append($button);

    $win->Add($vbox);
    $win->BorderWidthSet(5);
    $win->ShowAll();    
}

sub dnd_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Dnd Test");
    $win->Add(Etk::Label->new("<b>Etk::Dnd is not implemented yet.</b>"));
    
    $win->BorderWidthSet(10);
    $win->ShowAll();    
}

sub colorpicker_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Color Picker Test");
    my $cp = Etk::Colorpicker->new();
    $cp->UseAlphaSet(1);
    $win->Add($cp);
    
    $win->ShowAll();
}

sub filechooser_window_show
{
    my $win = Etk::Dialog->new();
    $win->TitleSet("Etk-Perl Filechooser Test");
    
    my $fc = Etk::Filechooser->new();
    $win->PackInMainArea($fc, BoxStart, BoxExpandFill, 0);
    $win->ButtonAdd("Open", 1);
    $win->ButtonAdd("Close", 2);

    $win->ShowAll();   
}

sub main_window_delete
{
    Etk::Main::quit();
}
