use strict;
use POSIX;
use Etk;
use Etk::Main;
use Etk::Window;
use Etk::VBox;
use Etk::HBox;
use Etk::Frame;
use Etk::Button;
use Etk::CheckButton;
use Etk::ToggleButton;
use Etk::Entry;
use Etk::Image;
use Etk::Label;
use Etk::Table;
use Etk::FillPolicy;
use Etk::HSlider;
use Etk::VSlider;
use Etk::ProgressBar;
use Etk::Timer;
use Etk::Theme;
use Etk::Tree;
use Etk::Tree::Col;
use Etk::Tree::Model::Int;
use Etk::Tree::Model::ProgressBar;
use Etk::Tree::Model::Image;
use Etk::Tree::Model::Double;
use Etk::Tree::Model::IconText;
use Etk::Tree::Model::Checkbox;

Etk::Init();

my $NUM_COLS = 2;
my $win = Etk::Window->new("Etk-Perl Test");
my $vbox_frames = Etk::VBox->new(0, 0);

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
	label => "menu",
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
    my $button = Etk::Button->new($value->{label});
    my $table = $frames{ $value->{frame} }->{table};
    my $j = $frames{ $value->{frame} }->{examples};
    $button->SignalConnect("clicked", $value->{cb});
    $table->AttachDefaults($button, $j % $NUM_COLS, $j % $NUM_COLS, 
	floor($j / $NUM_COLS), floor($j / $NUM_COLS));
    $frames{ $value->{frame} }->{examples}++;
}

$win->SignalConnect("delete_event", \&main_window_delete);
$win->Add($vbox_frames);
$win->BorderWidthSet(5);
$win->ShowAll();

Etk::Main::Run();
Etk::Shutdown();

sub button_window_show
{
    my $win = Etk::Window->new("Etk-Perl Button Test");
    my $vbox = Etk::VBox->new(0, 0);
    
    my $button = Etk::Button->new("Normal Button");
    $vbox->PackStart($button);
    
    $button = Etk::Button->new("Button with an image");    
    $button->ImageSet(Etk::Image->new("images/e_icon.png"));
    $vbox->PackStart($button);
    
    $button = Etk::Button->new();
    $vbox->PackStart($button);
    
    $button = Etk::CheckButton->new("Check Button");
    $vbox->PackStart($button);
    
    $button = Etk::CheckButton->new();
    $vbox->PackStart($button);
    
    #######
    # TODO: implement radio buttons!
    # $button = Etk::RadioButton->new("Normal Button");
    # $vbox->PackStart($button);
    #
    # $button = Etk::RadioButton->new();
    # $vbox->PackStart($button);
    #######
    
    $button = Etk::ToggleButton->new("Toggle Button");
    $vbox->PackStart($button);
    
    $button = Etk::ToggleButton->new();
    $vbox->PackStart($button);
    
    $win->Add($vbox);
    $win->ShowAll();
}

sub image_window_show
{
    my $win = Etk::Window->new("Etk-Perl Image Test");
    my $table = Etk::Table->new(2, 2, 0);
    my $image1 = Etk::Image->new("images/test.png");
    my $image2 = Etk::Image->new("images/test.png");
    my $label1 = Etk::Label->new("Keep aspect");
    my $label2 = Etk::Label->new("Don't keep aspect");
    
    $image1->KeepAspectSet(1);
    $image2->KeepAspectSet(0);
    
    $table->AttachDefaults($image1, 0, 0, 0, 0);
    $table->AttachDefaults($image2, 1, 1, 0, 0);
    $table->Attach($label1, 0, 0, 1, 1, 2, 0, Etk::FillPolicy::HExpand);
    $table->Attach($label2, 1, 1, 1, 1, 2, 0, Etk::FillPolicy::HExpand);    
    
    $win->Add($table);
    $win->ShowAll();
}

sub entry_window_show
{
    my $win = Etk::Window->new("Etk-Perl Entry Test");
    my $vbox = Etk::VBox->new(0, 0);    
    my $hbox = Etk::HBox->new(0, 0);
    my $entry = Etk::Entry->new();
    my $print_button = Etk::Button->new("Print text");
    my $toggle_button = Etk::ToggleButton->new("Toggle password");
    my $label = Etk::Label->new("");
    
    $print_button->SignalConnect("clicked", 
	sub {
	    $label->Set($entry->TextGet());
	}
    );
    
    $toggle_button->SignalConnect("clicked",
	sub {
	    $entry->PasswordSet(!$entry->PasswordGet());
	}
    );
    
    $hbox->PackStart($entry);
    $hbox->PackStart($print_button);
    $hbox->PackStart($toggle_button);    
    
    $vbox->PackStart($hbox);
    $vbox->PackStart($label);
    
    $win->Add($vbox);
    $win->ShowAll();
}

sub slider_window_show
{
    my $win = Etk::Window->new("Etk-Perl Slider Test");
    my $table = Etk::Table->new(2, 2, 0);
    my $slider1 = Etk::HSlider->new(0.0, 255.0, 128.0, 1.0, 10.0);
    my $slider2 = Etk::VSlider->new(0.0, 255.0, 128.0, 1.0, 10.0);
    my ($label1, $label2);
    
    $slider1->SizeRequestSet(130, 130);    
    $table->AttachDefaults($slider1, 0, 0, 0, 0);
    $label1 = Etk::Label->new("128.00");
    $table->Attach($label1, 0, 0, 1, 1, 0, 0, Etk::FillPolicy::None);
    $slider1->SignalConnect("value_changed",
	sub {
	    my $value = shift;
	    $label1->Set( sprintf("%.2f", $value));
	}
    );
    
    $slider2->SizeRequestSet(130, 130);    
    $table->AttachDefaults($slider2, 1, 1, 0, 0);       
    $label2 = Etk::Label->new("128.00");
    $table->Attach($label2, 1, 1, 1, 1, 0, 0, Etk::FillPolicy::None);
    $slider2->SignalConnect("value_changed",
	sub {
	    my $value = shift;
	    $label2->Set( sprintf("%.2f", $value));
	}
    );
    
    $win->BorderWidthSet(5);
    $win->Add($table);
    $win->ShowAll();    
}

sub progbar_window_show
{
    my $win = Etk::Window->new("Etk-Perl Progress Bar Test");
    my $vbox = Etk::VBox->new(1, 5);
    my $pbar1 = Etk::ProgressBar->new("0% done");
    my $pbar2 = Etk::ProgressBar->new("Loading...");
    
    $pbar2->PulseStepSet(0.015);
    
    $vbox->PackStart($pbar1);
    $vbox->PackStart($pbar2);

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
    
    $pbar1->SignalConnect("destroyed", 
	sub {
	    $timer1->Delete();
	}
    );
    
    my $timer2 = Etk::Timer->new(0.025,
	sub {
	    $pbar2->Pulse();
	    return 1;
	}
    );
    
    $pbar2->SignalConnect("destroyed", 
	sub {
	    $timer2->Delete();
	}
    );    
    
    $win->Add($vbox);
    $win->ShowAll();    
}

sub canvas_window_show
{
    my $win = Etk::Window->new("Etk-Perl Canvas Test");
    my $label = Etk::Label->new("<b>Etk::Canvas is not implemented yet.</b>");
    
    $win->Add($label);
    $win->BorderWidthSet(10);
    $win->ShowAll();
}

sub tree_window_show
{
    my $win = Etk::Window->new("Etk-Perl Tree Test");
    my $table = Etk::Table->new(2, 3, 0);
    my $label = Etk::Label->new("<h1>Tree:</h1>");
    
    $table->Attach($label, 0, 0, 0, 0, 0, 0, 
	Etk::FillPolicy::HFill | Etk::FillPolicy::VFill);
    
    my $tree = Etk::Tree->new();
    $tree->SizeRequestSet(320, 400);
    $table->AttachDefaults($tree, 0, 0, 1, 1);
    $tree->ModeSet(Etk::Tree::ModeTree);
    $tree->MultipleSelectSet(1);
    $tree->Freeze();
    
    my $col1 = Etk::Tree::Col->new($tree, "Column 1", 
	Etk::Tree::Model::IconText->new($tree,
	    Etk::Tree::Model::IconText::FromEdje), 90);
    
    my $col2 = Etk::Tree::Col->new($tree, "Column 2",
	Etk::Tree::Model::Double->new($tree), 60);
    
    my $col3 = Etk::Tree::Col->new($tree, "Column 3",
	Etk::Tree::Model::Image->new($tree, 
	    Etk::Tree::Model::Image::FromFile), 60);
    
    my $col4 = Etk::Tree::Col->new($tree,, "Column 4",
	Etk::Tree::Model::Checkbox->new($tree), 40);
    $col4->SignalConnect("cell_value_changed", 
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
	$row->FieldIconEdjeTextSet($col1, Etk::Theme::IconThemeGet(),
	    "places/user-home_16", "Row1");
	$row->FieldDoubleSet($col2, 10.0);
	$row->FieldImageFileSet($col3, "images/1star.png");
	$row->FieldCheckboxSet($col4, 0);
    }
    $tree->Thaw();
    
    $label = Etk::Label->new("<h1>List:</h1>");
    $table->Attach($label, 1, 1, 0, 0, 0, 0, 
	Etk::FillPolicy::HFill | Etk::FillPolicy::VFill);
    
    $tree = Etk::Tree->new();
    $tree->SizeRequestSet(320, 400);
    $table->AttachDefaults($tree, 1, 1, 1, 1);
    $tree->ModeSet(Etk::Tree::ModeList);
    $tree->MultipleSelectSet(1);
    $tree->Freeze();
    
    $col1 = Etk::Tree::Col->new($tree, "Column 1", 
	Etk::Tree::Model::IconText->new($tree,
	    Etk::Tree::Model::IconText::FromFile), 90);
    
    $col2 = Etk::Tree::Col->new($tree, "Column 2",
	Etk::Tree::Model::Int->new($tree), 90);
    $col2->SortFuncSet(\&tree_col2_compare_cb, "some_data");
    
    $col3 = Etk::Tree::Col->new($tree, "Column 3",
	Etk::Tree::Model::Image->new($tree, 
	    Etk::Tree::Model::Image::FromFile), 90);
    
    $tree->Build();        
    tree_add_items($tree, 500);
    my $frame = Etk::Frame->new("List Actions");
    $table->Attach($frame, 0, 1, 2, 2, 0, 0, 
	Etk::FillPolicy::HFill | Etk::FillPolicy::VFill);
    my $hbox = Etk::HBox->new(1, 10);
    $frame->Add($hbox);
    
    my $button = Etk::Button->new("Clear");
    $hbox->PackStart($button);
    
    $button = Etk::Button->new("Add 5 rows");
    $button->SignalConnect("clicked",
	sub {
	    tree_add_items($tree, 5);
	}
    );
    $hbox->PackStart($button);
    
    $button = Etk::Button->new("Add 50 rows");
    $button->SignalConnect("clicked",
	sub {
	    tree_add_items($tree, 50);
	}
    );    
    $hbox->PackStart($button);
    
    $button = Etk::Button->new("Add 500 rows");
    $button->SignalConnect("clicked",
	sub {
	    tree_add_items($tree, 500);
	}
    );    
    $hbox->PackStart($button);
    
    $button = Etk::Button->new("Add 5000 rows");
    $button->SignalConnect("clicked",
	sub {
	    tree_add_items($tree, 5000);
	}
    );    
    $hbox->PackStart($button);
    
    $button = Etk::Button->new("Sort");
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
    my $col1 = $tree->NthColGet(0);
    my $col2 = $tree->NthColGet(1);
    my $col3 = $tree->NthColGet(2);
    
    $tree->Freeze();
    for(my $i = 0; $i < $n; $i++)
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
    print "menu_window_show\n";
}

sub combobox_window_show
{
    print "combobox_window_show\n";
}

sub iconbox_window_show
{
    print "iconbox_window_show\n";
}

sub textview_window_show
{
    print "textview_window_show\n";
}

sub table_window_show
{
    print "table_window_show\n";
}

sub paned_window_show
{
    print "paned_window_show\n";
}

sub scrolledview_window_show
{
    print "scrolledview_window_show\n";
}

sub notebook_window_show
{
    print "notebook_window_show\n";
}

sub dnd_window_show
{
    print "dnd_window_show\n";
}

sub colorpicker_window_show
{
    print "colorpicker_window_show\n";
}

sub filechooser_window_show
{
    print "filechooser_window_show\n";
}

sub main_window_delete
{
    Etk::Main::Quit();
}
