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
    
    $vbox->PackStart($pbar1);
    $vbox->PackStart($pbar2);
    
    $win->Add($vbox);
    $win->ShowAll();    
}

sub canvas_window_show
{
    print "canvas_window_show\n";
}

sub tree_window_show
{
    print "tree_window_show\n";
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
