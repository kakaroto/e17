use strict;
use POSIX;
use Etk;
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
    $vbox_frames->PackStart($value->{widget});
    $value->{widget}->Add($value->{table});
    $value->{examples} = 0;
}

# Create buttons and attach them
while (my ($key, $value) = each %buttons)
{
    my $button = Etk::Button::new_with_label($value->{label});
    my $table = $frames{ $value->{frame} }->{table};
    my $j = $frames{ $value->{frame} }->{examples};
    $button->SignalConnect("clicked", $value->{cb});
    $table->AttachDefaults($button, $j % $NUM_COLS, $j % $NUM_COLS, 
	floor($j / $NUM_COLS), floor($j / $NUM_COLS));
    $frames{ $value->{frame} }->{examples}++;
}

$win->SignalConnect("delete_event", \&main_window_delete);
$win->BorderWidthSet(5);
$win->ShowAll();

Etk::Main::run();

sub button_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Button Test");
    my $vbox = Etk::VBox->new(0, 0);
   
    my $button = Etk::Button::new_with_label("Normal Button");
    $vbox->PackStart($button);
    $button = Etk::Button::new_with_label("Button with an image");
    $button->ImageSet(Etk::Image::new_from_file("images/e_icon.png"));
    $vbox->PackStart($button);
    $button = Etk::Button->new();
    $vbox->PackStart($button);
    $button = Etk::CheckButton::new_with_label("Check Button");
    $vbox->PackStart($button);
    $button = Etk::CheckButton->new();
    $vbox->PackStart($button);

    my $radio = Etk::RadioButton::new_with_label("Radio Button");
    $vbox->PackStart($radio);

    my $radio2 = Etk::RadioButton::new_from_widget($radio);
    $vbox->PackStart($radio2);

    my $toggle = Etk::ToggleButton::new_with_label("Toggle Button");
    $vbox->PackStart($toggle);
    $toggle = Etk::ToggleButton->new();
    $vbox->PackStart($toggle);

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
    
    $table->AttachDefaults($image1, 0, 0, 0, 0);
    $table->AttachDefaults($image2, 1, 1, 0, 0);
    $table->Attach($label1, 0, 0, 1, 1, 2, 0, HExpand);
    $table->Attach($label2, 1, 1, 1, 1, 2, 0, HExpand);    
    
    $win->Add($table);
    $win->ShowAll();
}

sub entry_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Entry Test");
    my $vbox = Etk::VBox->new(0, 0);    
    $win->Add($vbox);
    my $hbox = Etk::HBox->new(0, 0);
    $vbox->PackStart($hbox);
    my $label = Etk::Label->new("");
    $vbox->PackStart($label);

    my $entry = Etk::Entry->new();
    $hbox->PackStart($entry);

    my $button = Etk::Button->new();
    $button->LabelSet("Print text");
    $button->SignalConnect("clicked",
    	sub {
		$label->Set($entry->TextGet());
	}
	);
    $hbox->PackStart($button);

    my $button2 = Etk::ToggleButton->new();
    $button2->LabelSet("Toggle password");
    $button2->SignalConnect("clicked",
    	sub {
		$entry->PasswordSet(!$entry->PasswordGet());
	}
	);
    $hbox->PackStart($button2);

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
    $table->AttachDefaults($slider1, 0, 0, 0, 0);
    $label1 = Etk::Label->new("128.00");
    $table->Attach($label1, 0, 0, 1, 1, 0, 0, FillNone);
    $slider1->SignalConnect("value_changed",
	sub {
	    my $self = shift;
	    my $value = shift;
	    $label1->Set( sprintf("%.2f", $value));
	}
    );
    
    $slider2->SizeRequestSet(130, 130);    
    $table->AttachDefaults($slider2, 1, 1, 0, 0);       
    $label2 = Etk::Label->new("128.00");
    $table->Attach($label2, 1, 1, 1, 1, 0, 0, FillNone);
    $slider2->SignalConnect("value_changed",
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

    $pbar1 = Etk::ProgressBar::new_with_text("0% done");
    $pbar1->SignalConnect("destroyed",
    	sub {
		$timer1->Delete();
	});
    $vbox->PackStart($pbar1);
    $pbar2 = Etk::ProgressBar::new_with_text("Loading...");
    $pbar2->PulseStepSet(0.015);
    $pbar2->SignalConnect("destroyed",
    	sub {
		$timer2->Delete();
	});
    $vbox->PackStart($pbar2);
    
    
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
    my $table = Etk::Table->new(2, 3, 0);
    my $label = Etk::Label->new("<h1>Tree:</h1>");
    
    $table->Attach($label, 0, 0, 0, 0, 0, 0, 
	HFill | VFill);
    
    my $tree = Etk::Tree->new();
    $tree->SizeRequestSet(320, 400);
    $table->AttachDefaults($tree, 0, 0, 1, 1);
    $tree->ModeSet(ModeTree);
    $tree->MultipleSelectSet(1);
    $tree->Freeze();
    
    my @_cols = (
    	[ "Column 1", ["IconText", FromEdje], 90 ],
	[ "Column 2", "Double", 60 ],
	[ "Column 3", ["Image", FromFile], 60 ],
	[ "Column 4", "Checkbox", 40 ]
    );

    my $col1 = $tree->ColNew("Column 1", Etk::Tree::Model::IconText->new($tree, FromEdje), 90);
    my $col2 = $tree->ColNew("Column 2", Etk::Tree::Model::Double->new($tree), 60);
    my $col3 = $tree->ColNew("Column 3", Etk::Tree::Model::Image->new($tree, FromFile), 60);
    my $col4 = $tree->ColNew("Column 4", Etk::Tree::Model::Checkbox->new($tree), 40);

    $col2->SignalConnect("cell_value_changed", 
	sub {
	    # TODO: we need to implement etk_tree_row_fields_get
	    # why is this getting called if we're not clicking?
	    # print "toggle!\n";
	}
    );
    
    $tree->Build();

    for(my $i = 0; $i < 1000; $i++)
    {
	my $row = $tree->Append();
	$row->FieldIconEdjeTextSet($col1, Etk::Theme::icon_theme_get(), "places/user-home_16", "Row1");
	$row->FieldDoubleSet($col2, 10.0);
	$row->FieldImageFileSet($col3, "images/1star.png");
	$row->FieldCheckboxSet($col4, 0);
    }
    $tree->Thaw();
    
    $label = Etk::Label->new("<h1>List:</h1>");
    $table->Attach($label, 1, 1, 0, 0, 0, 0, HFill | VFill);
    
    $tree = Etk::Tree->new();
    $tree->SizeRequestSet(320, 400);
    $table->AttachDefaults($tree, 1, 1, 1, 1);
    $tree->ModeSet(ModeList);
    $tree->MultipleSelectSet(1);
    $tree->Freeze();
    
    $col1 = $tree->ColNew("Column 1", Etk::Tree::Model::IconText->new($tree, FromFile), 90);
    $col2 = $tree->ColNew("Column 2", Etk::Tree::Model::Int->new($tree), 90);
    $col3 = $tree->ColNew("Column 3", Etk::Tree::Model::Image->new($tree, FromFile), 90);

    my @cols = ($col1, $col2, $col3);


    $tree->Build();        
    tree_add_items($tree, 500, @cols);
    my $frame = Etk::Frame->new("List Actions");
    $table->Attach($frame, 0, 1, 2, 2, 0, 0, HFill | VFill);
    my $hbox = Etk::HBox->new(1, 10);

    $frame->Add($hbox); 
    
    $hbox->PackStart(Etk::Button::new_with_label("Clear"));

    my $button;

    $button = Etk::Button::new_with_label("Add 5 rows");
    $button->SignalConnect("clicked", sub { tree_add_items($tree, 5, @cols) });
    $hbox->PackStart($button);

    $button = Etk::Button::new_with_label("Add 50 rows");
    $button->SignalConnect("clicked", sub { tree_add_items($tree, 50, @cols) });
    $hbox->PackStart($button);

    $button = Etk::Button::new_with_label("Add 500 rows");
    $button->SignalConnect("clicked", sub { tree_add_items($tree, 500, @cols) });
    $hbox->PackStart($button);

    $button = Etk::Button::new_with_label("Add 5000 rows");
    $button->SignalConnect("clicked", sub { tree_add_items($tree, 5000, @cols) });
    $hbox->PackStart($button);

    
    my $ascendant = 1;

    $button = Etk::Button::new_with_label("Sort");
    
    $button->SignalConnect("clicked",
	sub {
	    #$tree->Sort(\&tree_col2_compare_cb, $ascendant, $col2, undef);
	    $tree->SortNumeric($ascendant, $col2, undef);
	    $ascendant = !$ascendant;
	}
    );
    $hbox->PackStart($button);    
    
    $win->Add($table);
    $win->ShowAll();
}

sub tree_col2_compare_cb
{
    my $tree = shift;
    my $row1 = shift;
    my $row2 = shift;
    my $col = shift;
    my $data = shift;
    
    my $v1 = $row1->FieldIntGet($col);
    my $v2 = $row2->FieldIntGet($col);
    
    if ($v1 < $v2)
    {
	return -1;
    }
    elsif ($v1 > $v2)
    {
	return 1;
    }
    else
    {
	return 0;
    }
    
    return 1;
}

sub tree_add_items
{
    my $tree = shift;
    my $n = shift;
    my ($col1, $col2, $col3) = @_;
    
    $tree->Freeze();
    for my $i (0 ..  $n)
    {
	my $row_name = "Row$i";
	my $star_path = "";
	if($i % 3 ==0)
	{
	    $star_path = "images/1star.png";
	} elsif($i % 3 == 1)
	{
	    $star_path = "images/2stars.png";
	} else
	{
	    $star_path = "images/3stars.png";
	}
	my $rand_value = rand(10000);

        
	my $row = $tree->Append();
	$row->FieldIconFileTextSet($col1, "images/1star.png", $row_name);
	$row->FieldIntSet($col2, $rand_value);
	$row->FieldImageFileSet($col3, $star_path);

    }
    $tree->Thaw();
	
}

sub menu_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Menu Test");
    $win->SizeRequestSet(300, 200);
    my $box = Etk::VBox->new(0, 0);

    my $menubar = Etk::Menu::Bar->new();

    $box->PackStart($menubar, 0, 0);

    my $label = Etk::Label->new("Click me :)");
    $label->AlignmentSet(0.5, 0.5);
    $label->PassMouseEventsSet(1);

    $box->PackStart($label);

    my $statusbar = Etk::StatusBar->new();

    $box->PackEnd($statusbar, 0, 0);

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
    $win->SignalConnect("mouse_down", sub { $menu->Popup() });
    
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
    my $menu_item = Etk::Menu::Item::new_with_label($label);
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->Push($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("deselected", 
    	sub { $statusbar->Pop(0) });
    
    return $menu_item;
}

sub _menu_test_stock_item_new
{
    my ($label, $stockid, $menubar, $statusbar) = @_;

    my $menu_item = Etk::Menu::Item::Image::new_with_label($label);
    my $image = Etk::Image::new_from_stock($stockid, SizeSmall);
    $menu_item->Set($image);
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->Push($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("deselected", 
    	sub { $statusbar->Pop(0) });

    return $menu_item;

}

sub _menu_test_check_item_new
{
    my ($label, $menubar, $statusbar) = @_;

    my $menu_item = Etk::Menu::Item::Check::new_with_label($label);
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->Push($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("deselected", 
    	sub { $statusbar->Pop(0) });

    return $menu_item;
}

sub _menu_test_radio_item_new
{
    my ($label, $radio, $menubar, $statusbar) = @_;

    my $menu_item;
    if ($radio) 
    {
        $menu_item = Etk::Menu::Item::Radio::new_with_label_from_widget($label, $radio);
    }
    else
    {
        $menu_item = Etk::Menu::Item::Radio::new_with_label($label);
    }
    $menubar->Append($menu_item);
    $menu_item->SignalConnect("selected", 
    	sub { $statusbar->Push($menu_item->LabelGet(), 0) });
    $menu_item->SignalConnect("deselected", 
    	sub { $statusbar->Pop(0) });

    return $menu_item;
}

sub _menu_seperator_new
{
    my ($menubar) = @_;
    my $menu_item = Etk::Menu::Item::Separator->new();
    $menubar->Append($menu_item);
    return $menu_item;
}


sub combobox_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Combo Test");
    my $vbox = Etk::VBox->new(0, 3);
    
    my $frame = Etk::Frame->new("Simple combobox");
    $vbox->PackStart($frame, 0, 0, 0);
    
    my $combobox = Etk::Combobox::new_default();
    $frame->Add($combobox);
    $combobox->ItemAppend("Test 1");
    $combobox->ItemAppend("Test 2");
    $combobox->ItemAppend("Test 3");    

    $frame = Etk::Frame->new("Some stock icons");
    $vbox->PackStart($frame);
    
    my $vbox2 = Etk::VBox->new(0, 3);
    $frame->Add($vbox2);
    
    my $image = Etk::Image::new_from_stock(DocumentNew, SizeBig);
    $vbox2->PackStart($image, 0, 0, 0);
    
    $combobox = Etk::Combobox->new();
    $combobox->ColumnAdd(ColumnTypeImage, 24, 0, 0, 0, 0.0, 0.5);
    $combobox->ColumnAdd(ColumnTypeLabel, 75, 1, 0, 0, 0.0, 0.5);
    $combobox->Build();
    
    $vbox2->PackStart($combobox, 0, 0, 0);
	
    for( my $i = DocumentNew; 
	$i <= FormatTextUnderline; $i++)
    {
	my $image2 = Etk::Image::new_from_stock($i, SizeSmall);
	my $item = $combobox->ItemAppend($image2, Etk::Stock::label_get($i));
#	$item->ColSet(1, Etk::Stock::label_get($i));
	$item->DataSet($i);
    }

    $combobox->ActiveItemSet($combobox->ActiveItemGet());
    
    $combobox->SignalConnect("active_item_changed", 
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
    
    $_iconbox_folder = "";    
    my $iconbox = Etk::Iconbox->new();
    my $model = Etk::Iconbox::Model->new($iconbox);
    $model->GeometrySet(150, 20, 2, 2);
    $model->IconGeometrySet(20, 0, 130, 16, 0.0, 0.5);
    _iconbox_folder_set($iconbox, "");
    
    $iconbox->SignalConnect("mouse_up", 
	sub {
	    my $self = shift;
	    my $event = shift;
	    my $icon = $iconbox->IconGetAtXy($event->{canvas_x},
		$event->{canvas_y}, 0, 1, 1);
	    return if($icon == undef);
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
    
    $folder = %ENV->{HOME} if($folder eq "");
    return if($folder eq "");      
    
    $iconbox->Clear();
    $iconbox->Append(Etk::Theme::icon_theme_get(), "actions/go-up_48", "..");
    
    # Add directories
    opendir(DIR, $_iconbox_folder."/".$folder) or 
      die "can't opendir $folder: $!";
    while (defined($file = readdir(DIR))) {
	if (-d "$_iconbox_folder/$folder/$file" && $file  !~ /^\./)
	{
	    $iconbox->Append(Etk::Theme::icon_theme_get(), 
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
		$iconbox->Append(Etk::Theme::icon_theme_get(), 
		    $_iconbox_types{$parts[-1]}, $file);
	    }
	    else
	    {
		$iconbox->Append(Etk::Theme::icon_theme_get(), 
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

    my $button = Etk::Button::new_with_label("Tag Presentation");
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


	    $vbox->PackStart($text_view);

	    $win->ShowAll();


    });
    $vbox->PackStart($button);

    $button = Etk::Button::new_with_label("Instant Messenger");
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
	    $vbox->PackStart($hbox, 0, 1, 0);

	    for my $b (FormatTextBold, FormatTextItalic, FormatTextUnderline) {
		    my $button = Etk::Button::new_from_stock($b);
		    $hbox->PackStart($button, 0, 1, 0);
	    }

	    my $editor_view = Etk::TextView->new();
	    $editor_view->SizeRequestSet(200, 80);
	    $vbox->PackStart($editor_view, 1, 1, 0);
	    my @buddies = (
		"<b><font color=#16569e>Jack B:</font></b> ",
		"<b><font color=#609028>Chloe O:</font></b> ",
		"<b><font color=#a82f2f>David P:</font></b> "
	    );
	    my $num_messages = 0;
	    $editor_view->SignalConnect("key_down", sub {
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
			    Etk::etk_signal_stop(); # FIXME fix signals
		    }

	    }, $message_view);

	    $win->ShowAll();
    });
    $vbox->PackStart($button);
    
    $win->ShowAll();  
}

sub table_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Table Test");
    
    my @widgets;
    
    push @widgets, Etk::Button::new_from_stock(DocumentOpen);
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
    
    push @widgets, Etk::Button::new_from_stock(DialogCancel);
    push @widgets, Etk::Button::new_from_stock(DocumentSave);
    
    push @widgets, Etk::Image::new_from_file("images/test.png");

    push @widgets, Etk::Alignment->new(0.5, 0.5, 0, 0);

    $widgets[20]->Add($widgets[0]);

    my $vbox = Etk::VBox->new(0, 0);
    my $hbox = Etk::HBox->new(0, 0);
    my $table = Etk::Table->new(2, 10, 0);

    $vbox->PackStart($table, 0, 0, 0);
    $vbox->PackEnd($hbox, 0, 0, 0);
    $hbox->PackEnd($widgets[18], 0, 0, 0);
    $hbox->PackEnd($widgets[17], 0, 0, 0);

    $table->Attach($widgets[19], 0, 0, 0, 0, 0, 0, FillNone);
    $table->Attach($widgets[20], 1, 1, 0, 0, 0, 0, HExpand | HFill);

    my $index = 1;
    for my $i (2 .. 9) 
    {
        $table->Attach($widgets[$index], 0, 0, $i, $i, 0, 0, HFill);
        $table->AttachDefaults($widgets[$index + 1], 1, 1, $i, $i);
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
    $vbox->PackStart($vpaned);

    my $hpaned = Etk::HPaned->new();
    $vpaned->Child1Set($hpaned, 0);

    my $label = Etk::Label->new("HPaned Child 1");
    $label->AlignmentSet(0.5, 0.5);
    $hpaned->Child1Set($label, 0);

    $label = Etk::Label->new("HPaned Child 2");
    $label->AlignmentSet(0.5, 0.5);
    $hpaned->Child2Set($label, 0);

    $label = Etk::Label->new("VPaned Child 2");
    $label->AlignmentSet(0.5, 0.5);
    $vpaned->Child2Set($label, 1);

    my $hseparator = Etk::HSeparator->new();
    $vbox->PackStart($hseparator, 0, 0, 6);

    # Properties Area
    my $hbox = Etk::HBox->new(1, 0);
    $vbox->PackStart($hbox, 0, 1, 0);

    my $frame = Etk::Frame->new("HPaned Properties");
    $hbox->PackStart($frame, 1, 1, 0);

    my $vbox2 = Etk::VBox->new(1, 0);
    $frame->Add($vbox2);
    
    my $check_button = Etk::CheckButton::new_with_label("Child 1 Expand");
    $check_button->ActiveSet(1);
    $vbox2->PackStart($check_button, 1, 1, 0);

    $check_button = Etk::CheckButton::new_with_label("Child 2 Expand");
    $vbox2->PackStart($check_button, 1, 1, 0);

    $frame = Etk::Frame->new("VPaned Properties");
    $hbox->PackStart($frame, 1, 1, 0);

    $vbox2 = Etk::VBox->new(1, 0);
    $frame->Add($vbox2);
    $check_button = Etk::CheckButton::new_with_label("Child 1 Expand");
    $vbox2->PackStart($check_button, 1, 1, 0);
    $check_button = Etk::CheckButton::new_with_label("Child 2 Expand");
    $check_button->ActiveSet(1);
    $vbox2->PackStart($check_button, 1, 1, 0);

    $win->Add($vbox);
    $win->ShowAll();
   
}

sub scrolledview_window_show
{
    my $win = Etk::Window->new();
    $win->TitleSet("Etk-Perl Scrolled View Test");
    $win->SizeRequestSet(180,180);

    my $scrolledview = Etk::ScrolledView->new();
    my $button = Etk::Button::new_with_label("Scrolled View Test");
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
    $vbox->PackStart($notebook, 1, 1, 0);

    my @widgets;
    
    push @widgets, Etk::Button::new_from_stock(DocumentOpen);
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
    
    push @widgets, Etk::Image::new_from_file("images/test.png");
    push @widgets, Etk::Alignment->new(0.5, 0.5, 0, 0);
    $widgets[18]->Add($widgets[0]);
    my $table = Etk::Table->new(2, 10, 0);
    $table->Attach($widgets[17], 0, 0, 0, 0, 0, 0, FillNone);
    $table->Attach($widgets[18], 1, 1, 0, 0, 0, 0, HExpand | HFill);

    my $index = 1;
    for my $i (2 .. 9) 
    {
        $table->Attach($widgets[$index], 0, 0, $i, $i, 0, 0, HFill);
        $table->AttachDefaults($widgets[$index + 1], 1, 1, $i, $i);
        $index += 2;
    }

    $notebook->PageAppend("Tab 1 - Table test", $table);

    my $alignment = Etk::Alignment->new(0.5, 0.5, 0.2, 0);
    my $vbox2 = Etk::VBox->new(0, 3);
    $alignment->Add($vbox2);
 
    my $button = Etk::Button::new_with_label("Normal Button");
    $vbox2->PackStart($button);
    
    $button = Etk::ToggleButton::new_with_label("Toggle Button");
    $vbox2->PackStart($button);
    
    $button = Etk::CheckButton::new_with_label("Check Button");
    $vbox2->PackStart($button);
    
    $button = Etk::CheckButton->new();
    $vbox2->PackStart($button);
    
    $button = Etk::RadioButton::new_with_label("Radio button");
    $vbox2->PackStart($button);

    my $button2 = Etk::RadioButton::new_from_widget($button);
    $vbox2->PackStart($button2);
    
    $notebook->PageAppend("Tab 2 - Button test", $alignment);

    $vbox->PackStart(Etk::HSeparator->new(), 0, 0, 5);

    $alignment = Etk::Alignment->new(0.5, 0.5, 0, 1);
    $vbox->PackStart($alignment, 0, 0, 0);
    my $hbox = Etk::HBox->new(1, 0);
    $alignment->Add($hbox);

    $button = Etk::Button::new_from_stock(GoPrevious);
    $button->LabelSet("Previous");
    $button->SignalConnect("clicked", sub {
		$notebook->PagePrev();
    });
    $hbox->PackStart($button, 0, 1, 0);
    
    $button = Etk::Button::new_from_stock(GoNext);
    $button->LabelSet("Next");
    $button->SignalConnect("clicked", sub {
		$notebook->PageNext();
    });
    $hbox->PackStart($button, 0, 1, 0);

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
    $win->Add(Etk::Colorpicker->new());
    
    $win->ShowAll();
}

sub filechooser_window_show
{
    my $win = Etk::Dialog->new();
    $win->TitleSet("Etk-Perl Filechooser Test");
    
    my $fc = Etk::Filechooser->new();
    $win->PackInMainArea($fc, 1, 1, 0, 0);
    $win->ButtonAdd("Open", 1);
    $win->ButtonAdd("Close", 2);

    $win->ShowAll();   
}

sub main_window_delete
{
    Etk::Main::quit();
}
